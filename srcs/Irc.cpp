/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 18:06:38 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/12 00:09:25 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

// bool init_global = 1;

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
    for (size_t i = 0; i < _chanel.size(); i++) {
        delete _chanel[i];
    }
    for (size_t i = 0; i < _client.size(); i++) {
        delete _client[i];
    }
    return ;
}

void    Irc::closeServer()
{
    while(!_client.empty())
        _client.erase(_client.begin());

    while(!_chanel.empty())
        _chanel.erase(_chanel.begin());
    close(epoll_fd);
    close(server_fd);
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

int Irc::getServerFd() const { return (server_fd); }
		
int Irc::getEpollFd() const { return (epoll_fd); }
		
struct epoll_event Irc::getEvent(int i) const { return (events[i]); } 
		
struct epoll_event* Irc::getEventTab() {  return (events);  }

void    Irc::addClient(int client_fd)
{
	static unsigned int			token = 1;
	std::stringstream		op;
	std::string				ret;

	op << token;
	op >> ret;
	
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    event.data.fd = client_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1 )
    {
        perror("epoll_ctl EPOLL_CTL_ADD client soscket");
        exit(EXIT_FAILURE);
    }
    _client.push_back((new Client(client_fd, ret)));

	sendMessagetoClient(_client.back(), CMD_PING(ret));
	token++;
}

void     Irc::eraseClient(Client* client)
{
    for (std::size_t i = 0; i < _client.size(); i++)
    {
        if (client->getMySocket() == _client[i]->getMySocket())
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->getMySocket(), NULL ) == -1)
            {
                perror("epoll_ctl EPOLL_CTL_DEL");
                exit(EXIT_FAILURE);			
            }
            std::cout << RED << "Client[ " << CYAN << client->getMySocket() << RED << " ] deconnected !" << RESET << std::endl;
            for (Client::map_iterator it = client->getChanelMap().begin(); it != client->getChanelMap().end(); it++)
            {
                if (it->first->getNumClient() == 1)
                {
                    Chanel *chanel_ptr = findChanel(it->first->getChanelName());
                    for (std::size_t i = 0; i <_chanel.size(); i++)
                        if (_chanel[i]->getChanelName() == it->first->getChanelName())
                        {
                            _chanel.erase(_chanel.begin() + i);
                            delete chanel_ptr;
                            break ;
                        }
                }
            }
            close(client->getMySocket());
            _client.erase(_client.begin() + i);
        }
    }
}

void        Irc::sendMessagetoClient(Client* client, std::string msg)
{
    int			bytes_sent;
    int 		len = msg.size();

    if ((bytes_sent = send(client->getMySocket(), msg.c_str(), len, 0 )) != len)
    {
        perror("send failed");
        throw std::runtime_error("Error when sending a message to the client");
        client->setLastActiveTime();
    }
}

bool        Irc::goodPassword(Client* client, std::string pswd)
{
    std::string msg;

    if (pswd == server_pswd)
            return (1);
    msg  = std::string(RED) + "Error: " + std::string(RESET) + "Wrong password !\n";
    sendMessagetoClient(client, msg);
    return (0);
}

bool Irc::isNicknameAvaible(std::string nickname)
{
    for(size_t i = 0; i < _client.size(); i++)
        if (nickname == _client[i]->getMyNickname())
            return (0);
    return (1);
}

bool Irc::parsInfo(Client* client, std::vector<std::string> info)
{
    std::string msg;

    if (info[2] == "PASS")
    {
        std::string pswd(info[3]);
        if (goodPassword(client, pswd) == false)
            return (false);
    }
    else
    {
        msg = std::string(RED) + "Error: " + std::string(RESET) + "No password has been entered!\n";
        sendMessagetoClient(client, msg);
        return (false);
    }
    std::string nick(info[5]);
    if (isNicknameAvaible(nick) == 0)
    {
        sendMessagetoClient(client, ERR_NICKNAMEINUSE(nick));
        nick.push_back('_');
    }
    client->setNickName(nick);

    std::string user(info[7]);
    client->setUserName(user);

    if (client->getMyNickname() == "otabchi" || client->getMyNickname() == "asimon" || client->getMyNickname() == "idouidi")
    {
        std::cout << std::string(GREEN)  << "SERVER OPERATOR : [" << std::string(YELLOW) << client->getMyNickname() << std::string(GREEN) << "] JOIN THE SERVER" << std::string(RESET) << std::endl;
        client->setModes(SERVER_OPERATOR);
    }

    // client->setStatusClient(0);

    sendMessagetoClient(client, RPL_WELCOME(client->getMyNickname()) + \
     RPL_YOURHOST(client->getMyNickname(), "Les Oubliés 1.0.0")+ \
     RPL_CREATED(client->getMyNickname(), getDateTime())+ \
     RPL_MYINFO(client->getMyNickname(), "Les Oubliés 1.0.0"));
    return (true);
}

bool Irc::setClientMode(Client* client, std::string cmd, char mode)
{
    if (client->setModes(mode))
    {
        sendMessagetoClient(client, SET_CLIENT_MODE(client->getMyNickname(), client->getMyUserName(), cmd, mode));
        return (true);
    }
    return (false);
}

bool Irc::unsetClientMode(Client* client, std::string cmd, char mode)
{
    if (client->unsetModes(mode))
    {
        sendMessagetoClient(client, UNSET_CLIENT_MODE(client->getMyNickname(), client->getMyUserName(), cmd, mode));
        return (true);
    }
    return (false);
}

bool Irc::setChanelMode(Client* client, Chanel* chanel, std::string cmd, char mode)
{
    if (chanel->setModes(mode))
    {
        sendMessagetoClient(client, SET_CHANEL_MODE(client->getMyNickname(), client->getMyUserName(), cmd, chanel->getChanelName(), mode));
        return (true);
    }
    return (false);
}

bool Irc::unsetChanelMode(Client* client, Chanel* chanel, std::string cmd, char mode)
{
    if (chanel->unsetModes(mode))
    {
        sendMessagetoClient(client, UNSET_CHANEL_MODE(client->getMyNickname(), client->getMyUserName(), cmd, chanel->getChanelName(), mode));
        return (true);
    }
    return (false);
}

Client*     Irc::findClient(int fd_client)
{
    for (size_t i = 0; i < _client.size(); i++)
    {
        if (_client[i]->getMySocket() == fd_client)
            return (_client[i]);
    }
    return NULL;
}

Client*     Irc::findClient(std::string nick)
{
    for (size_t i = 0; i < _client.size(); i++)
    {
        if (_client[i]->getMyNickname() == nick)
            return (_client[i]);
    }
    return (NULL);
}

Chanel*     Irc::findChanel(std::string chanel_name)
{
    for (size_t i = 0; i < _chanel.size(); i++)
    {
        if (_chanel[i]->getChanelName() == chanel_name)
            return (_chanel[i]);
    }
    return (NULL);
}

std::string Irc::clientLastActiveTime(Client* client)
{
    char buffer[20];
    int idle_time = time(0) - client->getLastActiveTime();
    sprintf(buffer, "%d", idle_time); // conversion de la valeur en chaîne de caractères
    return (std::string(buffer));  
}

bool        Irc::execCmd(Client* client , std::vector<std::string> cmd)
{
    bool (Irc::*tab[])(Client*, std::vector<std::string> ) = {&Irc::ping,
																&Irc::pong,
                                                                &Irc::mode,
                                                                &Irc::who,
                                                                &Irc::whois,
                                                                &Irc::whowas,
                                                                &Irc::join,
                                                                &Irc::part,
                                                                &Irc::privatemsg,
                                                                &Irc::list,
                                                                &Irc::nick,
                                                                &Irc::quit,
                                                                &Irc::invite,
                                                                &Irc::kick
                                                                };
    std::string ref[] = {"PING", 
						"PONG",
                        "MODE",
                        "WHO",
                        "WHOIS",
                        "WHOWAS",
                        "JOIN",
                        "PART",
                        "PRIVMSG",
                        "LIST",
                        "NICK",
                        "QUIT",
                        "INVITE",
                        "KICK",
                        "NULL"};

    std::size_t size = 0;
    while (ref[size] != "NULL")
        size++;

    for(std::size_t i = 0; i < size; i++)
    {
        if (ref[i] == cmd[0])
        {
            (this->*(tab[i]))(client, cmd);
            return (true);
        }
    }
    sendMessagetoClient(client, ERR_UNKNOWNCOMMAND(client->getMyNickname(), cmd[0]));
    return (false);
}

bool Irc::ping(Client* client, std::vector<std::string> cmd)
{
    (void)cmd;
    sendMessagetoClient(client, RPL_PONG);
    return (true);
}

bool	Irc::pong(Client* client, std::vector<std::string> cmd)
{
	if (cmd[1] != ":" + client->getToken())
		sendMessagetoClient(client, ERR_BADPING(client->getMyNickname()));
	return true;
}

bool Irc::mode(Client* client, std::vector<std::string> cmd)
{
    //MODE FOR A CLIENT
    if (cmd[1][0] != '#')
    {
        if (client->isNewClient() == 0 && client->isServerModeActivated(SERVER_OPERATOR) == 0)
        {
            sendMessagetoClient(client, ERR_NOPRIVILEGES(client->getMyNickname()));
            return (false);
        }
        for (std::size_t i = 0; i < cmd[2].size(); i++)
        {
            if (setClientMode(client, cmd[0], cmd[2][i]))
                ;
            else if (cmd[2][i + 1])
            {
                if (cmd[2][i] == '+' && setClientMode(client, cmd[0], cmd[2][i + 1]))
                    i++;
                else if (cmd[2][i] == '-' && unsetClientMode(client, cmd[0], cmd[2][i + 1]))
                    i++;
                else
                    sendMessagetoClient(client, ERR_UMODEUNKNOWNFLAG(client->getMyNickname()));
            }
            else
                sendMessagetoClient(client, ERR_UMODEUNKNOWNFLAG(client->getMyNickname()));
        }
        client->setStatusClient(0);
    }
    //MODE FOR A CHANEL
    else
    {
        Chanel* current_chanel;
        current_chanel = findChanel(cmd[1]);
        if (current_chanel == NULL)
        {
            sendMessagetoClient(client, ERR_NOSUCHCHANNEL(client->getMyNickname(), cmd[1]));
            return (false);
        }
        if (cmd.size() == 3)
        {
            if (cmd[2] == "b")
                sendMessagetoClient(client, RPL_ENDOFBANLIST(client->getMyNickname(), cmd[1]));
            else
            {
                // Chanel::map_iterator it = current_chanel->getClient(client->getMyNickname()), ite = current_chanel->getclientMap().end();
                // if (init_global == 0)
                //     if ((client->isServerModeActivated(SERVER_OPERATOR) == 0 ) || (it != ite && current_chanel->isClientModeActivated(it->second, CHANEL_OPERATOR)))
                //     {
                //         sendMessagetoClient(client, ERR_CHANOPRIVSNEED(client->getMyNickname(), current_chanel->getChanelName()));
                //         return (false);
                //     }
                bool before_setting_modes = current_chanel->isChanelModeActivated(INVITE_ONLY);
                for (std::size_t i = 0; i < cmd[2].size(); i++)
                {
                    if (setChanelMode(client, current_chanel, cmd[0], cmd[2][i]))
                        ;
                    else if (cmd[2][i + 1])
                    {
                        if (cmd[2][i] == '+' && setChanelMode(client, current_chanel, cmd[0], cmd[2][i + 1]))
                            i++;
                        else if (cmd[2][i] == '-' && unsetChanelMode(client, current_chanel, cmd[0], cmd[2][i + 1]))
                            i++;
                        else
                            sendMessagetoClient(client, ERR_UNKNOWNMODE(client->getMyNickname(), cmd[2][i + i]));
                    }
                    else
                        sendMessagetoClient(client, ERR_UNKNOWNMODE(client->getMyNickname(), cmd[2][i + i]));
                }
                if (before_setting_modes == false && current_chanel->isChanelModeActivated(INVITE_ONLY))
                {
                    for (Chanel::map_iterator it = current_chanel->getclientMap().begin(), ite = current_chanel->getclientMap().end(); it != ite; it++)
                        current_chanel->getWhiteList().push_back(it->first->getMyNickname());
                }
            }
        }
        else
        {
            time_t t = time(0);
            std::stringstream		ss;
	        std::string				stime;
            std::string             listChanelMode;

	        ss << t;
	        ss >> stime;

            listChanelMode = current_chanel->listChanelModes();
            sendMessagetoClient(client, RPL_CHANNELMODEIS(client->getMyNickname(), cmd[1], + listChanelMode)
            + RPL_CREATIONTIME(client->getMyNickname(), cmd[1], stime));
        }
        // init_global = 0;
    }
    return (true);
}

bool Irc::who(Client* client, std::vector<std::string> cmd)
{
    std::string modes;

    if (cmd[1][0] == '#')
    {
        Chanel      *potential_chanel = findChanel(cmd[1]);
        if (potential_chanel == NULL)
        {
            sendMessagetoClient(client, RPL_ENDOFWHO(client->getMyNickname(), cmd[1]));
            return (false);
        }
        for(Chanel::map_iterator it =  potential_chanel->getclientMap().begin(); it != potential_chanel->getclientMap().end(); it++)
        {
		    for (std::size_t i = 0; i < it->second.size(); i++)
		    {
                if (it->second[i] == SERVER_OPERATOR)
                    modes += '*';
		    	else if (it->second[i] == CHANEL_OPERATOR)
		    		modes += '@';
		    	else if (it->second[i] == VOICE)
		    		modes += '+';
		    }
            sendMessagetoClient(client, RPL_WHOREPLY(client->getMyNickname(), cmd[1], client->getMyUserName(), it->first->getMyNickname(), "H", modes));
            modes.clear();
        }
        sendMessagetoClient(client, RPL_ENDOFWHO(client->getMyNickname(), cmd[1]));
    }
    else
    {
        Client  *potential_client = findClient(cmd[1]);
        if (potential_client == NULL)
        {
            sendMessagetoClient(client, RPL_ENDOFWHO(client->getMyNickname(), cmd[1]));
            return (false);      
        }
        else if (potential_client->getChanelMap().size() == 0) // IF THE CLIENT JOIN NO CHANELS
        {
            //IF THE CLIENT HAS NO SPECIAL MODES (LIKES INVISIBLE AT START)
            if ((modes = potential_client->listClientServerModes()) == "i")
                sendMessagetoClient(client, RPL_WHOREPLY_BIS(client->getMyNickname(), "*", client->getMyUserName(), potential_client->getMyNickname(), "H")
                + RPL_ENDOFWHO(client->getMyNickname(), cmd[1]));
            else
                sendMessagetoClient(client, RPL_WHOREPLY(client->getMyNickname(), "*", client->getMyUserName(), potential_client->getMyNickname(), "H", modes)
                + RPL_ENDOFWHO(client->getMyNickname(), cmd[1]));
        }
        else
        {
            for (Client::map_iterator it = potential_client->getChanelMap().begin(); it != potential_client->getChanelMap().end(); it++)
                sendMessagetoClient(client, RPL_WHOREPLY(client->getMyNickname(), it->first->getChanelName(), client->getMyUserName(), potential_client->getMyNickname() , "H", potential_client->listClientChanelModes(it->second)));
        }
    }
    return (1);
}

bool Irc::whois(Client* client, std::vector<std::string> cmd)
{
    if (isNicknameAvaible(cmd[1]) != 0)
    {
        sendMessagetoClient(client, ERR_NOSUCHNICK(client->getMyNickname(), cmd[1]) + RPL_ENDOFWHOIS(client->getMyNickname(), cmd[1]));
        return (false);
    }
    else
    {
        Client *current_client = findClient(cmd[1]);
        if (current_client)
        {
            sendMessagetoClient(current_client,
            RPL_WHOISUSER(client->getMyNickname(), current_client->getMyNickname(), current_client->getMyUserName())
            + RPL_WHOISSERVER(client->getMyNickname(), current_client->getMyNickname())
            + RPL_WHOISIDLE(client->getMyNickname(), current_client->getMyNickname(), clientLastActiveTime(client))
            + RPL_ENDOFWHOIS(client->getMyNickname(), current_client->getMyNickname()));
        }
    }
    return (true);    
}

bool Irc::whowas(Client* client, std::vector<std::string> cmd)
{
    Client *current_client = findClient(client->getMyNickname());

    if (current_client == NULL)
        sendMessagetoClient(client, ERR_WASNOSUCHNICK(client->getMyNickname(), cmd[1]) 
        + RPL_ENDOFWHOWAS(client->getMyNickname(), cmd[1]));
    return (true);
}

bool Irc::join(Client* client, std::vector<std::string> cmd)
{
    Chanel *current_chanel;
    bool founder = 0;

    current_chanel = findChanel(cmd[1]);
    if (current_chanel == NULL)
    {
        _chanel.push_back((new Chanel(cmd[1]))); // ( _id = _chanel.size() + 1 )
        current_chanel = _chanel[_chanel.size() - 1];
        current_chanel->setModes('n');
        current_chanel->setModes('t');
        current_chanel->setChanelName(cmd[1]);
        founder = 1;
    }
    
    if (current_chanel->isPresentInList(current_chanel->getBlackList(), client->getMyNickname()))
        return (0);
    else if (current_chanel->getWhiteList().size() > 0 
            && current_chanel->isPresentInList(current_chanel->getWhiteList(), client->getMyNickname()) == 0)
    {
        sendMessagetoClient(client, ERR_INVITEONLYCHAN(client->getMyNickname(), current_chanel->getChanelName()));
        return (0);
    }
    else if (current_chanel->isPresentInChanel(client->getMyNickname()))
        return (0);
    else
    {   
        std::vector<client_mode>	client_mode_in_chanel; // Need to do 
        if (client->isServerModeActivated(SERVER_OPERATOR))
            client_mode_in_chanel.push_back(SERVER_OPERATOR);
        if (founder == 1)
			client_mode_in_chanel.push_back(CHANEL_OPERATOR);
        else
			client_mode_in_chanel.push_back(NON_CLIENT_MODE);
		current_chanel->addClient(client, client_mode_in_chanel);
        client->insertChanel(current_chanel, client_mode_in_chanel);        
        //DEBUG PART
        // CheckChanelInfo(current_chanel);
    }
    
    std::string list = current_chanel->listAllClientsModesAndNames();
    sendMessagetoClient(client, SET_CHANEL(client->getMyNickname(), client->getMyUserName(), cmd[0], cmd[1])
    + RPL_NAMREPLY(client->getMyNickname(), cmd[1], list) + RPL_ENDOFNAMES(client->getMyNickname(), cmd[1]));
    for (Chanel::map_iterator it = current_chanel->getclientMap().begin(), ite = current_chanel->getclientMap().end(); it != ite; it++)
    {
        if (it->first->getMyNickname() == client->getMyNickname())
            continue;
        sendMessagetoClient(const_cast<Client*>(it->first), JOINING_MSG(client->getMyNickname(), it->first->getMyUserName(), cmd[1]));
    }
    return (1);
}

bool Irc::part(Client* client, std::vector<std::string> cmd)
{
    Chanel *current_chanel;
    if ((current_chanel = findChanel(cmd[1])) == NULL)
    {
        sendMessagetoClient(client, ERR_NOSUCHCHANNEL(client->getMyNickname(), cmd[1]));
        return (false);
    }
    // IF SUCCESS TO DELETE CLIENT IN CHANEL MAP, I CAN DELETE CHANEL IN CLIENT MAP
    else if (current_chanel->deleteClient(client->getMyNickname()))
    {
        client->deleteChanel(current_chanel->getChanelName());
        if (current_chanel->getclientMap().size() == 0)
        {
            for (std::size_t i = 0; i < _chanel.size(); i++)
            {
                if (_chanel[i]->getChanelName() == current_chanel->getChanelName())
                {
                    _chanel.erase(_chanel.begin() + i);
                    break;
                }
            }            
        }
        sendMessagetoClient(client, PART_CHANEL(client->getMyNickname(), client->getMyUserName(), cmd[0], cmd[1]));
    for (Chanel::map_iterator it = current_chanel->getclientMap().begin(), ite = current_chanel->getclientMap().end(); it != ite; it++)
    {
        if (it->first->getMyNickname() == client->getMyNickname())
            continue;
        sendMessagetoClient(const_cast<Client*>(it->first), LEAVE_MSG(client->getMyNickname(), it->first->getMyUserName(), cmd[1]));
    }
        return (true);
    }
    else
        sendMessagetoClient(client, ERR_NOSUCHCHANNEL(client->getMyNickname(), cmd[1]));
    return (false);
}

bool    Irc::privatemsg(Client* client, std::vector<std::string> cmd) 
{
    Chanel *current_chanel = findChanel(cmd[1]);
    if (current_chanel)
    {
         Chanel::map_iterator it = current_chanel->getClient(client->getMyNickname()), ite = current_chanel->getclientMap().end();
        if (current_chanel->isChanelModeActivated(MODERATED) && it != ite && current_chanel->isClientModeActivated(it->second, VOICE))
        {
            for (Chanel::map_iterator it = current_chanel->getclientMap().begin(), ite = current_chanel->getclientMap().end(); it != ite; it++)
            {
                if (it->first->getMyNickname() == client->getMyNickname())
                    continue;
                sendMessagetoClient(const_cast<Client*>(it->first), RPL_PRIVMSG(client->getMyNickname(), client->getMyUserName(), "PRIVMSG", current_chanel->getChanelName(), cmd[2]));
            }
        }
        else
        {
            sendMessagetoClient(client, ERR_CANNOTSENDTOCHAN(client->getMyNickname(), current_chanel->getChanelName()));
            return (false);
        }
    }
    return (true);
}

bool Irc::list(Client* client, std::vector<std::string> cmd)
{
    std::stringstream		ss;
	std::string				num;
    (void)cmd;
    sendMessagetoClient(client, RPL_LISTSTART(client->getMyNickname()));
    for (std::size_t i = 0; i < _chanel.size(); i++)
    {
        if (_chanel[i]->isChanelModeActivated(SECRET))
        {
            continue;
        }
	    ss << _chanel[i]->getNumClient();
	    ss >> num;
        sendMessagetoClient(client, RPL_LIST(client->getMyNickname(), _chanel[i]->getChanelName(), num));
    }
    sendMessagetoClient(client, RPL_LISTEND(client->getMyNickname()));
    return (1);
}

bool Irc::nick(Client* client, std::vector<std::string> cmd)
{
    std::string msg;
    if (cmd.size() == 1)
    {
        sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));
        return (false);
    }
    else if (isNicknameAvaible(cmd[1]) == 0)
    {
        sendMessagetoClient(client, ERR_NICKNAMEINUSE(cmd[1]));
        return (false);
    }
    client->setNickName(cmd[1]);
    return (true);

}

bool Irc::invite(Client* client, std::vector<std::string> cmd)
{
    Client *potential_client = findClient(cmd[1]);

    if (potential_client == NULL)
    {
        sendMessagetoClient(client, ERR_NOSUCHNICK(client->getMyNickname(), cmd[1]));
        return (false);
    }
    Chanel *current_chanel = findChanel(cmd[2]);
    current_chanel->getWhiteList().push_back(potential_client->getMyNickname());
    sendMessagetoClient(client, INVITE_CLIENT(client->getMyNickname(), client->getMyUserName(), cmd[0], potential_client->getMyNickname(), cmd[2]));
    sendMessagetoClient(potential_client, NOCTICE_CLIENT_INVITE(client->getMyNickname(), current_chanel->getChanelName()));

    return (true);
}

bool Irc::quit(Client* client, std::vector<std::string> cmd)
{
    if (cmd.size() > 1)
    {
        std::cout << RED << "error: " << RESET << "Usage: /quit" << std::endl;
        return (0);
    }
    eraseClient(client);
    return (1);
}

bool    Irc::kick(Client* client, std::vector<std::string> cmd)
{
    Chanel *current_chanel = findChanel(cmd[1]);
    Client *current_client;
    if (current_chanel)
    {
        // Chanel::map_iterator it = current_chanel->getClient(client->getMyNickname()), ite = current_chanel->getclientMap().end();
        // if ((client->isServerModeActivated(SERVER_OPERATOR) == 0 ) || (it != ite && current_chanel->isClientModeActivated(it->second, CHANEL_OPERATOR)))
        // {
        //     sendMessagetoClient(client, ERR_CHANOPRIVSNEED(client->getMyNickname(), current_chanel->getChanelName()));
        //     return (false);            
        // }
        if ((current_client = findClient(cmd[2])) == NULL)
        {
            sendMessagetoClient(client, ERR_NOSUCHNICK(client->getMyNickname(), cmd[2]));
            return (false);          
        }
        else if (current_chanel->isPresentInChanel(cmd[2]) == 0)
        {
            sendMessagetoClient(client, ERR_USERNOTINCHANNEL(client->getMyNickname(), cmd[2], cmd[1]));
            return (false);
        }
        for (Chanel::map_iterator it = current_chanel->getclientMap().begin(), ite = current_chanel->getclientMap().end(); it != ite; it++)
            sendMessagetoClient(const_cast<Client*>(it->first), KIK_CLIENT(client->getMyNickname(), client->getMyUserName(), cmd[0], cmd[1], cmd[2]));

        current_chanel->deleteClient(current_client->getMyNickname());
        current_client->deleteChanel(current_chanel->getChanelName());
        return (true);
    }
    return (false);
}

void Irc::CheckChanelInfo(Chanel* chanel)
{
    std::cout << std::string(RED) + "=  =   =   =   =   =   =   =   =   =   =   =   =   =   =   =   =   =   =   =" << std::string(RESET) << std::endl;

    for (Chanel::map_iterator it = chanel->getclientMap().begin(); it != chanel->getclientMap().end(); it++)
    {
        for (std::size_t i = 0; i < _client.size(); i++)
        {
            if ((it->first) == (_client[i]))
            {
                std::cout << "same address for the object in std::vector<client> and in a chanel instance class in std::map<Client, client_mode>: " << _client[i]->getMyNickname() << std::endl;
                break ;
            }
            else if ((it->first->getMyNickname()) == (_client[i]->getMyNickname()))
            {
                std::cout << "concerned client is = " <<_client[i]->getMyNickname() << " | " << it->first->getMyNickname() << std::endl; 
                std::cout <<  "differrent address for the object\nobject in std::vector<client> = " << &(it->first)\
                 << "\nobject in a chanel instance class in std::map<Client, client_mode> = " << &(_client[i]) << std::endl;
                break;
            }
        }
    }
    std::cout << std::string(RED) + "=  =   =   =   =   =   =   =   =   =   =   =   =   =   =   =   =   =   =   =" << std::string(RESET) << std::endl;
}

