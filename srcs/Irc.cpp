/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 18:06:38 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/30 16:04:20 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"


Irc::Irc(char *port, char *password)
{
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket");
        throw std::runtime_error("Error when creating the socket");
    }
    server_pswd = password;
    epoll_fd = -1;
    memset(&event, 0, sizeof(event));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(std::atoi(port));
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
    try {
        init_server();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

Irc::~Irc() 
{
    closeServer();
    return ;
}

void     Irc::init_server()
{
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAX_CLIENT) < 0)
    {
        perror("listen");
        throw std::runtime_error("Error when listening to the socket");
    }
    if ((epoll_fd = epoll_create(MAX_CLIENT)) == -1 )
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    event.data.fd = server_fd;
    event.events = EPOLLIN; //data can be read from a fd (server_fd) monitored by epoll 
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1 )
    {
        perror("epoll_ctl EPOLL_CTL_ADD server soscket");
        exit(EXIT_FAILURE);
    }
}

int Irc::getServerFd() const 
{ 
    return (server_fd); 
}
		
int Irc::getEpollFd() const 
{ 
    return (epoll_fd); 
}
		
struct epoll_event Irc::getEvent(int i) const 
{ 
    return (events[i]); 
} 
		
struct epoll_event* Irc::getEventTab() 
{ 
    return (events); 
}

void    Irc::addClient(int client_fd)
{
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    event.data.fd = client_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1 )
    {
        perror("epoll_ctl EPOLL_CTL_ADD client soscket");
        exit(EXIT_FAILURE);
    }
    _client.push_back(Client(client_fd));
}

void     Irc::eraseClient(Client& client)
{
    for (std::size_t i = 0; i < _client.size(); i++)
    {
        if (client.getMySocket() == _client[i].getMySocket())
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client.getMySocket(), NULL ) == -1)
            {
                perror("epoll_ctl EPOLL_CTL_DEL");
                exit(EXIT_FAILURE);			
            }
            std::cout << RED << "Client[ " << CYAN << client.getMySocket() << RED << " ] deconnected !" << RESET << std::endl;
            close(client.getMySocket());
            _client.erase(_client.begin() + i);
        }
    }
}

bool Irc::parsInfo(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    std::size_t position;

    position = cmd[1].find("PASS");
    if (position + 5 != std::string::npos)
    {
        std::string expression(cmd[1], 5, std::string::npos);
        std::cout << "exp = [" << expression << "]" << std::endl; 
        if (expression != server_pswd)
        {
            std::cout << "cmd[1] is " << cmd[1] << " | ["<< expression << "] is the wrong password" << std::endl;
            return (false);
        }
    }
    else
        return (false);
    std::string nick(cmd[2], 5, std::string::npos);
    client.setNickName(nick);
    client.setStatusClient(0);
    sendMessagetoClient(RPL_WELCOME(client.getMyNickname()));
    sendMessagetoClient(RPL_YOURHOST(client.getMyNickname(), "my.server.name"));
    sendMessagetoClient(RPL_CREATED(client.getMyNickname(), "today"));
    sendMessagetoClient(RPL_MYINFO(client.getMyNickname(), "my.server.name", "1.0", "avaible"));
    return (true);
}


void        Irc::sendMessagetoClient(int client_fd, std::string msg)
{
    int			bytes_sent;
    int 		len = msg.size();

    if ((bytes_sent = send(client_fd, msg.c_str(), len, 0 )) != len)
    {
        perror("send failed");
        throw std::runtime_error("Error when sending a message to the client");
    }
}

std::string     Irc::recvMessageFromClient(int client_fd)
{
    int			bytes_recv;
    char		buf[BUFFER_SIZE] = {0};
    std::string s_buf;
    if ((bytes_recv = recv(client_fd, buf, BUFFER_SIZE, 0 )) < 0)
    {
        perror("recv failed");
        throw std::runtime_error("Error when receving a message to the client");
    }
    if (buf[0])
    {
        s_buf = buf;
        s_buf.erase(s_buf.size() - 1);
    }
    else
        return ("");
    return (s_buf);
}


bool        Irc::goodPassword(Client& client, std::string pswd)
{
    std::string msg;
    if (pswd != "\n")
    {
        pswd.erase(pswd.size() - 1);
        if (pswd == server_pswd)
        {
            return (1);
        }
    }
    msg  = std::string(RED) + "Error: " + std::string(RESET) + "Wrong password !\n";
    sendMessagetoClient(client.getMySocket(), msg);
    eraseClient(client);
    return (0);
}

Client*     Irc::findClient(int fd_client)
{
    for (size_t i = 0; i < _client.size(); i++)
    {
        if (_client[i].getMySocket() == fd_client)
            return (&_client[i]);
    }
    return NULL;
}

bool        Irc::execCmd(Client& client , std::string cmd)
{
    if (cmd == "\n")
        return (0);
    cmd.erase(cmd.size() - 1);
    bool (Irc::*tab[7])(Client&, std::vector<std::string> ) = {&Irc::msg, &Irc::join, &Irc::leave, &Irc::list, &Irc::nick, &Irc::quit, &Irc::who};
    std::string ref[] = {"/msg", "/join", "/leave", "/list", "/nick", "/quit", "/who" };
    std::vector<std::string> split;
    std::stringstream ss(cmd);
    std::string sub_string;

    while (std::getline(ss, sub_string, ' '))
        split.push_back(sub_string);

    if (split[0][0] != '/' && std::isalnum(split[0][1]))
    {
        std::string msg = std::string(RED) + "Error: " + std::string(RESET) + "Invalid command format\n";
        sendMessagetoClient(client.getMySocket(), msg);
        return (0);
    }
    for (std::size_t i = 0; i < 7 ; i++)
    {
        if (ref[i] == split[0])
        {
            (this->*(tab[i]))(client, split);
            return (1);
        }
    }
    std::string msg = std::string(RED) + "Error: " + std::string(RESET) + "command not found\n";
    sendMessagetoClient(client.getMySocket(), msg);
    return (0);
}


void    Irc::closeServer()
{
    while(!_client.empty())
        _client.erase(_client.begin());

    while(!_chanel.empty())
        _chanel.erase(_chanel.begin());
    close(server_fd);
}


bool Irc::isNicknameAvaible(std::string nickname)
{
    if (nickname == "\n")
        return (0);
    for(size_t i = 0; i < _client.size(); i++)
        if (nickname == _client[i].getMyNickname())
            return (0);
    return (1);
}

bool Irc::msg(Client& client, std::vector<std::string> cmd)
{
    std::string msg;
    if (cmd.size() < 3)
    {
        msg = std::string(RED) + "Error: " + std::string(RESET) + "Invalid command format\n";
        return (false);
    }
    for (size_t i = 0; i < _client.size(); i++)
    {
        if (cmd[1] == _client[i].getMyNickname())
        {
            if (client.getMyNickname() == _client[i].getMyNickname())
			{
				msg = std::string(RED) + "Error: " +  std::string(RESET) + "You can't send a message to yourself!";
				sendMessagetoClient(_client[i].getMySocket(), msg);
				return (false);
			}
            msg = std::string(YELLOW) + "[" + std::string(CYAN) + client.getMyNickname() + std::string(YELLOW ) + "]"\
			+ std::string(CYAN) + " >" + std::string(RESET);
            sendMessagetoClient(_client[i].getMySocket(), msg);

            for (size_t j = 2 ; j < cmd.size(); j++ )
                sendMessagetoClient(_client[i].getMySocket(), " " + std::string(YELLOW) + cmd[j] + std::string(RESET));

            sendMessagetoClient(_client[i].getMySocket(), "\n");
            return (true);
        }
    }
	msg = std::string(RED) + "Error: " +  std::string(RESET) +  std::string(YELLOW) + "["\
	+ std::string(CYAN) + cmd[1] + std::string(YELLOW ) + "]" + std::string(RESET) +\
	" is not referenced on the server.";
	sendMessagetoClient(client.getMySocket(), msg);
    return (false);
}

bool Irc::join(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    (void)cmd;
    return (1);
}
bool Irc::leave(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    (void)cmd;
    return (1);
}
bool Irc::list(Client& client, std::vector<std::string> cmd)
{

    (void)client;
    (void)cmd;
    return (1);
}
bool Irc::nick(Client& client, std::vector<std::string> cmd)
{
    std::string msg;
    if (cmd.size() != 2)
    {
        msg = std::string(RED) + "Error: " + std::string(RESET) + "Invalid command format\n";
        sendMessagetoClient(client.getMySocket(), msg);
        return (false);
    }
    // if (cmd[1] == "")
    // {
    //     std::cout << "[" << cmd[1] << "]" << std::endl;
    //     msg =  std::string(RED) + "Error: " +  std::string(RESET) + "nickname can't be empty !\n";
    //     sendMessagetoClient(client.getMySocket(), msg);
    //     return (false);
    // }
    else if (isNicknameAvaible(cmd[1]))
    {
        client.setNickName(cmd[1]);
        msg = std::string(GREEN) + "You have changed your nickname to " + std::string(YELLOW) + "["\
	    + std::string(CYAN) + cmd[1] + std::string(YELLOW ) + "]" + std::string(RESET);
        sendMessagetoClient(client.getMySocket(), msg);
        return (true);
    }
    else
    {
        msg = std::string(YELLOW) + "[" + std::string(RED) + cmd[1] + std::string(YELLOW) + "] " + std::string(RESET) \
		+ "is already used by an another user :\\";
        sendMessagetoClient(client.getMySocket(), msg);
        return (false);
    }
}
bool Irc::quit(Client& client, std::vector<std::string> cmd)
{
    if (cmd.size() > 1)
    {
        std::cout << RED << "error: " << RESET << "Usage: /quit" << std::endl;
        return (0);
    }
    eraseClient(client);
    return (1);
}
bool Irc::who(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    (void)cmd;
    return (1);
}
