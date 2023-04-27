/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 18:06:38 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/27 16:04:00 by idouidi          ###   ########.fr       */
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
	if (client == NULL)
		return ;
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
			delete client;
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
	if (!checkInfo(client,info))
		return (false);
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
		
		// O = SERVER OPERATOR | o = CHANEL OPERATOR
		client->setServerModes('O');
	}
	sendMessagetoClient(client, RPL_WELCOME(client->getMyNickname()) + \
	 RPL_YOURHOST(client->getMyNickname(), "Les Oubliés 1.0.0")+ \
	 RPL_CREATED(client->getMyNickname(), getDateTime())+ \
	 RPL_MYINFO(client->getMyNickname(), "Les Oubliés 1.0.0"));
	client->setStatusClient(0);
	return (true);
}

bool Irc::setClientMode(Client* client, std::string cmd, char mode)
{
	if (client->setServerModes(mode))
	{
		sendMessagetoClient(client, SET_CLIENT_MODE(client->getMyNickname(), client->getMyUserName(), cmd, mode));
		return (true);
	}
	return (false);
}

bool Irc::unsetClientMode(Client* client, std::string cmd, char mode)
{
	if (client->unsetServerModes(mode))
	{
		sendMessagetoClient(client, UNSET_CLIENT_MODE(client->getMyNickname(), client->getMyUserName(), cmd, mode));
		return (true);
	}
	return (false);
}

bool Irc::setChanelMode(Client* client, Chanel* chanel, std::string cmd, char mode)
{
	if (chanel->setChanelModes(mode))
	{
		sendMessagetoClient(client, SET_CHANEL_MODE(client->getMyNickname(), client->getMyUserName(), cmd, chanel->getChanelName(), mode));
		return (true);
	}
	return (false);
}

bool Irc::unsetChanelMode(Client* client, Chanel* chanel, std::string cmd, char mode)
{
	if (chanel->unsetChanelModes(mode))
	{
		sendMessagetoClient(client, UNSET_CHANEL_MODE(client->getMyNickname(), client->getMyUserName(), cmd, chanel->getChanelName(), mode));
		return (true);
	}
	return (false);
}

bool Irc::setOtherClientChanelMode(Client* client, Client* other, Chanel* chanel, std::string cmd, char mode)
{
	if (mode == 'b')
	{
		std::vector<std::string>::iterator it = std::find(chanel->getBlackList().begin(), chanel->getBlackList().end(), other->getMyNickname());
		if (it == chanel->getBlackList().end())
			chanel->getBlackList().push_back(other->getMyNickname());
		sendMessagetoClient(client, SET_OTHER_CLIENT_CHANEL_MODE(client->getMyNickname(), client->getMyUserName(), cmd, chanel->getChanelName(), mode,  other->getMyNickname()));
		return (true);
		
	}
	Chanel::map_iterator it = chanel->getClient(other->getMyNickname());
	if (chanel->setClientMode(it, mode))
	{
		sendMessagetoClient(client, SET_OTHER_CLIENT_CHANEL_MODE(client->getMyNickname(), client->getMyUserName(), cmd, chanel->getChanelName(), mode,  other->getMyNickname()));
		return (true);
	}
	return (false);
}

bool Irc::unsetOtherClientChanelMode(Client* client, Client* other, Chanel* chanel, std::string cmd, char mode)
{
	if (mode == 'b')
	{
		std::vector<std::string>::iterator it = std::find(chanel->getBlackList().begin(), chanel->getBlackList().end(), other->getMyNickname());
		if (it != chanel->getBlackList().end())
			chanel->getBlackList().erase(it);
		sendMessagetoClient(client, UNSET_OTHER_CLIENT_CHANEL_MODE(client->getMyNickname(), client->getMyUserName(), cmd, chanel->getChanelName(), mode,  other->getMyNickname()));
		return (true);
		
	}
	Chanel::map_iterator it = chanel->getClient(other->getMyNickname());
	if (chanel->unsetClientMode(it, mode))
	{
		sendMessagetoClient(client, UNSET_OTHER_CLIENT_CHANEL_MODE(client->getMyNickname(), client->getMyUserName(), cmd, chanel->getChanelName(), mode,  other->getMyNickname()));
		return (true);
	}
	return (false);
}

std::string Irc::listClientModes(std::vector<client_mode>& client_mode)
{
	std::string list;
	for (std::size_t i = 0; i < client_mode.size(); i++)
	{
		if (client_mode[i] == SERVER_OPERATOR)
			list += "*";
		else if (client_mode[i] == CHANEL_OPERATOR)
			list += '@';
		else if (client_mode[i] == VOICE)
			list += '+';
	}
	return (list);
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
																&Irc::kick,
																&Irc::notice,
																&Irc::topic
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
						"NOTICE",
						"TOPIC",
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
	if (cmd.size() < 2)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}
	if (cmd[1] != ":" + client->getToken())
		sendMessagetoClient(client, ERR_BADPING(client->getMyNickname()));
	return true;
}

bool Irc::mode(Client* client, std::vector<std::string> cmd)
{
	if (cmd.size() < 2)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}
	//SET MODE FOR A CLIENT
	if (cmd[1][0] != '#')
	{
		if (cmd.size() < 3)
		{
			sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
			return (false);        
		}
		Client* concerned_client = findClient(cmd[1]);
		//TO INIT THE MODE AT THE CONENCTION
		if (client->getInitStatus() == true)
		{
			//CHECK IF THE CLIENT INIT HIS OWN MODE AT THE CONNECTION
			if (client != concerned_client)
			{
				std::string msg  = std::string(RED) + "Error: " + std::string(RESET) + "You must first init your mode at the connection!\n";
				sendMessagetoClient(client, msg);
				return (false);
			}
			//NEXT TIME THE CLIENT MUST BE SERVER OPERATOR TO SET MODE OF OTHER CLIENT IN THE SERVER
			client->setInitStatus(0);
		}
		// CHECK IF CONCERNED CLIENT EXIST
		else if (concerned_client == NULL)
		{
			sendMessagetoClient(client,  ERR_NOSUCHNICK(client->getMyNickname(), cmd[1]));
			return (false);
		}
		// CHECK IF I HAVE THE PRIVILEGE
		else if (client->isServerModeActivated(SERVER_OPERATOR) == 0)
		{
			sendMessagetoClient(client, ERR_NOPRIVILEGES(client->getMyNickname()));
			return (false);
		}
		//SET MODE FOR CLIENT IN THE SERVER
		for (std::size_t i = 0; i < cmd[2].size(); i++)
		{
			if (setClientMode(client, cmd[0], cmd[2][i]))
				continue ;
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
	}
	//SET MODE FOR A CHANEL / SET MODE FOR A CLIENT IN THE CHANEL
	else
	{
		Chanel* current_chanel;
		current_chanel = findChanel(cmd[1]);
		if (current_chanel == NULL)
		{
			sendMessagetoClient(client, ERR_NOSUCHCHANNEL(client->getMyNickname(), cmd[1]));
			return (false);
		}
		// CHANEL INFO
		if (cmd.size() == 2)
		{
			time_t t = time(0);
			std::stringstream		ss;
			std::string				stime;
			std::string				listChanelMode;

			ss << t;
			ss >> stime;

			listChanelMode = current_chanel->listChanelModes();
			sendMessagetoClient(client, RPL_CHANNELMODEIS(client->getMyNickname(), cmd[1], + listChanelMode)
			+ RPL_CREATIONTIME(client->getMyNickname(), cmd[1], stime));
		}
		// SET MODE FOR CHANEL
		else if (cmd.size() == 3)
		{
			//ASK FOR BLACK LIST
			if (cmd[2] == "b")
				sendMessagetoClient(client, RPL_ENDOFBANLIST(client->getMyNickname(), cmd[1]));
			else
			{
				Chanel::map_iterator	client_in_chanel = current_chanel->getClient(client->getMyNickname());
				Chanel::map_iterator	chanel_end = current_chanel->getclientMap().end();
				Chanel::map_iterator	chanel_begin = current_chanel->getclientMap().begin();

				// CHECK IF I HAVE THE PRIVILEGE
				if (client->isServerModeActivated(SERVER_OPERATOR) == 0)
				{
					// IF YOUR ARE NOT SERVER OPERATOR YOU MUST BE IN THE CHANEL AND BE CHANEL OPERATOR
					if ((client_in_chanel == chanel_end) || current_chanel->isClientModeActivated(client_in_chanel->second, CHANEL_OPERATOR) == 0)
					{
						sendMessagetoClient(client, ERR_CHANOPRIVSNEED(client->getMyNickname(), current_chanel->getChanelName()));
						return (false);
					}
				}

				// CHECK IF WE ACTIVE A WHITE LIST
				bool before_setting_modes = current_chanel->isChanelModeActivated(INVITE_ONLY);
				
				// SET MODES IN CHANEL
				for (std::size_t i = 0; i < cmd[2].size(); i++)
				{
					if (setChanelMode(client, current_chanel, cmd[0], cmd[2][i]))
						continue;
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
				
				// RECHECK IF WE ACTIVE A WHITE LIST . IF ITS TRUE WE ADD ALL CLIENT ALREADY PRESENT IN THE CHANEL IN
				if (before_setting_modes == false && current_chanel->isChanelModeActivated(INVITE_ONLY))
				{
					for (; chanel_begin != chanel_end; chanel_begin++)
						current_chanel->getWhiteList().push_back(chanel_begin->first->getMyNickname());

					// ADD THE SERVER OPERATOR IN THE WHITE LIST
					current_chanel->getWhiteList().push_back("asimon");
					current_chanel->getWhiteList().push_back("otabchi");
					current_chanel->getWhiteList().push_back("idouidi");
				}
			}
		}
		// SET MODE FOR A CLIENT IN THE CHANEL
		else if (cmd.size() == 4)
		{
			Chanel::map_iterator	client_in_chanel = current_chanel->getClient(client->getMyNickname());
			Chanel::map_iterator	chanel_end = current_chanel->getclientMap().end();

			// CHECK IF I HAVE THE PRIVILEGE
			if (client->isServerModeActivated(SERVER_OPERATOR) == 0)
			{
				// IF YOUR ARE NOT SERVER OPERATOR YOU MUST BE IN THE CHANEL AND BE CHANEL OPERATOR
				if ((client_in_chanel == chanel_end) || current_chanel->isClientModeActivated(client_in_chanel->second, CHANEL_OPERATOR) == 0)
				{
					sendMessagetoClient(client, ERR_CHANOPRIVSNEED(client->getMyNickname(), current_chanel->getChanelName()));
					return (false);
				}
			}
			
			Client *concerned_client = findClient(cmd[3]);
			// CONCERNED CLIENT DOES NOT EXIST
			if (concerned_client == NULL)
			{
				sendMessagetoClient(client, ERR_NOSUCHNICK(client->getMyNickname(), cmd[3]));
				return (false);
			}
			
			// CONCERNED CLIENT IS NOT IN THIS CHANEL
			else if (cmd[2] != "+b" && cmd[2] != "-b" && current_chanel->getClient(cmd[3]) == chanel_end)
			{
				sendMessagetoClient(client, ERR_USERNOTINCHANNEL(client->getMyNickname(), concerned_client->getMyNickname(), cmd[1]));
				return (false);
			}

			// SET MODES FOR THE CONCERNED CLIENT IN THE CHANEL
			for (std::size_t i = 0; i < cmd[2].size(); i++)
			{
				if (setOtherClientChanelMode(client, concerned_client, current_chanel, cmd[0], cmd[2][i]))
					continue ;
				else if (cmd[2][i + 1])
				{
					if (cmd[2][i] == '+' && setOtherClientChanelMode(client, concerned_client, current_chanel, cmd[0], cmd[2][i + 1]))
						i++;
					else if (cmd[2][i] == '-' && unsetOtherClientChanelMode(client, concerned_client, current_chanel, cmd[0], cmd[2][i + 1]))
						i++;
					else
						sendMessagetoClient(client, ERR_UMODEUNKNOWNFLAG(client->getMyNickname()));
				}
				else
					sendMessagetoClient(client, ERR_UMODEUNKNOWNFLAG(client->getMyNickname()));
			}
		}
	}
	return (true);
}

bool Irc::who(Client* client, std::vector<std::string> cmd)
{
	std::string modes;
	if (cmd.size() < 2)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}
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
			modes = listClientModes(it->second);
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
			if ((modes = listClientModes(potential_client->getActiveModes())) == "")
				sendMessagetoClient(client, RPL_WHOREPLY_BIS(client->getMyNickname(), "*", client->getMyUserName(), potential_client->getMyNickname(), "H")
				+ RPL_ENDOFWHO(client->getMyNickname(), cmd[1]));
			else
				sendMessagetoClient(client, RPL_WHOREPLY(client->getMyNickname(), "*", client->getMyUserName(), potential_client->getMyNickname(), "H", modes)
				+ RPL_ENDOFWHO(client->getMyNickname(), cmd[1]));
		}
		else
		{
			for (Client::map_iterator it = potential_client->getChanelMap().begin(); it != potential_client->getChanelMap().end(); it++)
				sendMessagetoClient(client, RPL_WHOREPLY(client->getMyNickname(), it->first->getChanelName(), client->getMyUserName(), potential_client->getMyNickname() , "H",  listClientModes(it->second)));
			sendMessagetoClient(client, RPL_ENDOFWHO(client->getMyNickname(), cmd[1]));
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
	if (cmd.size() < 2)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}
	Client *current_client = findClient(client->getMyNickname());

	if (current_client == NULL || current_client == client)
		sendMessagetoClient(client, ERR_WASNOSUCHNICK(client->getMyNickname(), cmd[1]) 
		+ RPL_ENDOFWHOWAS(client->getMyNickname(), cmd[1]));
	return (true);
}

bool Irc::join(Client* client, std::vector<std::string> cmd)
{
	if (cmd.size() < 2)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}
	Chanel *current_chanel;
	bool founder = 0;

	current_chanel = findChanel(cmd[1]);

	// IF THE CHANEL DOES NOT EXIST, CREATE THE CHANEL AND SET HIS MODE	
	if (current_chanel == NULL)
	{
		_chanel.push_back((new Chanel(cmd[1]))); // ( _id = _chanel.size() + 1 )
		current_chanel = _chanel[_chanel.size() - 1];
		current_chanel->setChanelModes('n');
		current_chanel->setChanelModes('t');
		current_chanel->setChanelModes('b');
		current_chanel->setChanelName(cmd[1]);
		founder = 1;
	}
	// CHECK IF THE CLIENT IS IN THE CHQNEL BLACK LIST
	if (current_chanel->isPresentInList(current_chanel->getBlackList(), client->getMyNickname()))
	{
		sendMessagetoClient(client, ERR_BANNEDFROMCHAN(client->getMyNickname(), current_chanel->getChanelName()));
		return (0);
	}
	// CHECK IF THE CLIENT IN IN THE WHITE LIST
	else if (current_chanel->getWhiteList().size() > 0 &&  current_chanel->isPresentInList(current_chanel->getWhiteList(), client->getMyNickname()) == 0) 
	{
		sendMessagetoClient(client, ERR_INVITEONLYCHAN(client->getMyNickname(), current_chanel->getChanelName()));
		return (0);
	}
	// CHECK IF THE CLIENT IS ALREQDY IN THE CHANEL
	else if (current_chanel->isPresentInChanel(client->getMyNickname()))
		return (0);
	else
	{   
		std::vector<client_mode>	client_mode_in_chanel;

		//SET THE CLIENT MODE IN THE CHANEL
		if (client->isServerModeActivated(SERVER_OPERATOR))
		{
			client_mode_in_chanel.push_back(SERVER_OPERATOR);
			client_mode_in_chanel.push_back(VOICE);
		}
		else if (founder == 1)
		{
			client_mode_in_chanel.push_back(CHANEL_OPERATOR);
			client_mode_in_chanel.push_back(VOICE);
		}

		// ADD THE CLIENT IN THE CHANEL MAP
		current_chanel->addClient(client, client_mode_in_chanel);
		// ADD THE CHANEL IN THE CLIENT MAP 
		client->insertChanel(current_chanel, client_mode_in_chanel);        
	}

	std::string list = current_chanel->listAllClientsModesAndNames(&Irc::listClientModes);

	sendMessagetoClient(client, SET_CHANEL(client->getMyNickname(), client->getMyUserName(), cmd[0], cmd[1])
	+ RPL_NAMREPLY(client->getMyNickname(), cmd[1], list) + RPL_ENDOFNAMES(client->getMyNickname(), cmd[1]));

	// SEND TO OTHER CLIENT THERE IS Q NEW CLIENT IN THE CHANEL
	for (Chanel::map_iterator it = current_chanel->getclientMap().begin(), ite = current_chanel->getclientMap().end(); it != ite; it++)
	{
		if (it->first->getMyNickname() == client->getMyNickname())
			continue;
		sendMessagetoClient(const_cast<Client*>(it->first), JOINING_MSG(client->getMyNickname(), client->getMyUserName(), cmd[0], current_chanel->getChanelName()));
	}
	return (1);
}

bool Irc::part(Client* client, std::vector<std::string> cmd)
{
	if (cmd.size() < 2)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}

	Chanel *current_chanel;
	// CHECK IF THE CHANEL EXIST
	if ((current_chanel = findChanel(cmd[1])) == NULL)
	{
		sendMessagetoClient(client, ERR_NOSUCHCHANNEL(client->getMyNickname(), cmd[1]));
		return (false);
	}

	Chanel::map_iterator	client_in_chanel = current_chanel->getClient(client->getMyNickname());
	Chanel::map_iterator	chanel_end = current_chanel->getclientMap().end();
	Chanel::map_iterator	chanel_begin = current_chanel->getclientMap().begin();

	// CHECK IF THE CLIENT IS IN THE CHANEL
	if (client_in_chanel != chanel_end)
	{
		for (; chanel_begin != chanel_end; chanel_begin++)
			sendMessagetoClient(const_cast<Client*>(chanel_begin->first), PART_CHANEL(client->getMyNickname(), client->getMyUserName(), cmd[0], current_chanel->getChanelName()));
		
		// DELETE THE CLIENT IN CHANEL MAP 
		current_chanel->deleteClient(client->getMyNickname());

		// DELETE THE CHANEL INT THE CLIENT MAP
		client->deleteChanel(current_chanel->getChanelName());

		// AFTER DELETED CLIENT WE CHECK IF THERE ARE STILL CLIENT IN 
		if (current_chanel->getclientMap().size() == 0)
		{
			// FIND THE CHANEL TO ERASE IT IN THE CHANEL REFERENCIAL
			for (std::size_t i = 0; i < _chanel.size(); i++)
			{
				if (_chanel[i]->getChanelName() == current_chanel->getChanelName())
				{
					_chanel.erase(_chanel.begin() + i);
					break;
				}
			}            
		}
		return (true);
	}
	else
		sendMessagetoClient(client, ERR_NOTONCHANNEL(client->getMyNickname(), cmd[1]));
	return (false);
}

bool    Irc::privatemsg(Client* client, std::vector<std::string> cmd) 
{
	if (cmd.size() < 3)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}
	
	// SET THE MSG CORRECLTY
	std::string msg;
	for (std::vector<std::string>::iterator it = cmd.begin() + 2; it != cmd.end(); ++it) 
	{
		msg += *it;
		std::vector<std::string>::iterator cpy = it;
		if (++cpy != cmd.end())
			msg += " ";
	}

	// SEND MSG TO A CLIENT
	if (cmd[1][0] != '#')
	{
		Client *current_client = findClient(cmd[1]);
		if (current_client)
			sendMessagetoClient(current_client, RPL_PRIVMSG_CLIENT(client->getMyNickname(), client->getMyUserName(), "PRIVMSG", cmd[1], msg));
		else
		{
			sendMessagetoClient(client, ERR_NOSUCHNICK(client->getMyNickname(), msg));
			return (false);
		}
	}
	//SEND MSG IN A CHANEL 
	else
	{
		Chanel *current_chanel = findChanel(cmd[1]);
		if (current_chanel)
		{
			Chanel::map_iterator	client_in_chanel = current_chanel->getClient(client->getMyNickname());
			Chanel::map_iterator	chanel_begin = current_chanel->getclientMap().begin();
			Chanel::map_iterator	chanel_end = current_chanel->getclientMap().end();

			// CHECK IF CLIENT IS IN THE CHANEL
			if (client_in_chanel == chanel_end)
			{
				sendMessagetoClient(client, ERR_CANNOTSENDTOCHAN(client->getMyNickname(), current_chanel->getChanelName()));
				return (false);				
			}
			// CHECK IF I HAVE PRIVILEGE 
			else if (current_chanel->isChanelModeActivated(MODERATED) && current_chanel->isClientModeActivated(client_in_chanel->second, VOICE) == 0)
			{
				sendMessagetoClient(client, ERR_CANNOTSENDTOCHAN(client->getMyNickname(), current_chanel->getChanelName()));
				return (false);
			}

			for (; chanel_begin != chanel_end; chanel_begin++)
			{
				if (chanel_begin->first->getMyNickname() == client->getMyNickname())
					continue;
				sendMessagetoClient(const_cast<Client*>(chanel_begin->first), RPL_PRIVMSG_CHANEL(client->getMyNickname(), client->getMyUserName(), "PRIVMSG", current_chanel->getChanelName(), msg));
			}
		}
		else
		{
			sendMessagetoClient(client, ERR_NOSUCHCHANNEL(client->getMyNickname(), cmd[1]));
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
	if (cmd.size() < 3)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}

	Client *potential_client = findClient(cmd[1]);
	if (potential_client == NULL)
	{
		sendMessagetoClient(client, ERR_NOSUCHNICK(client->getMyNickname(), cmd[1]));
		return (false);
	}

	Chanel *current_chanel = findChanel(cmd[2]);
	if (current_chanel == NULL)
	{
		sendMessagetoClient(client, ERR_NOSUCHCHANNEL(client->getMyNickname(), cmd[2]));
		return (false);
	}
	else if (current_chanel->getClient(client->getMyNickname()) == current_chanel->getclientMap().end())
	{
		sendMessagetoClient(client, ERR_USERNOTINCHANNEL(client->getMyNickname(), client->getMyNickname(), cmd[2]));
		return (false);
	}
	if (current_chanel->isChanelModeActivated(INVITE_ONLY))
		current_chanel->getWhiteList().push_back(potential_client->getMyNickname());
	sendMessagetoClient(client, INVITE_CLIENT(client->getMyNickname(), client->getMyUserName(), cmd[0], potential_client->getMyNickname(), cmd[2]));
	sendMessagetoClient(potential_client, NOCTICE_CLIENT_INVITE(client->getMyNickname(), current_chanel->getChanelName()));
	return (true);
}

bool Irc::quit(Client* client, std::vector<std::string> cmd)
{
	(void)cmd;
	eraseClient(client);
	return (1);
}

bool    Irc::kick(Client* client, std::vector<std::string> cmd)
{
	if (cmd.size() < 3)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}
	Chanel *current_chanel = findChanel(cmd[1]);
	if (current_chanel)
	{
		Client 					*current_client;
		Chanel::map_iterator	client_in_chanel = current_chanel->getClient(client->getMyNickname());
		Chanel::map_iterator	chanel_end = current_chanel->getclientMap().end();
		Chanel::map_iterator	chanel_begin = current_chanel->getclientMap().begin();

		// CHECK IF THE CONCERNED CLIENT IS ON THE SERVER
		if ((current_client = findClient(cmd[2])) == NULL)
		{
			sendMessagetoClient(client, ERR_NOSUCHNICK(client->getMyNickname(), cmd[2]));
			return (false);          
		}

		// CHECK IF THE CONCERNED CLIENT IS ON THE CHANEL
		else if (current_chanel->isPresentInChanel(cmd[2]) == 0)
		{
			sendMessagetoClient(client, ERR_USERNOTINCHANNEL(client->getMyNickname(), cmd[2], cmd[1]));
			return (false);
		}

		// CHECK IF I HAVE PRIVILEGE
		else if (client->isServerModeActivated(SERVER_OPERATOR) == 0)
		{
			// IF YOUR ARE NOT SERVER OPERATOR YOU MUST BE IN THE CHANEL AND BE CHANEL OPERATOR
			if ((client_in_chanel == chanel_end) || current_chanel->isClientModeActivated(client_in_chanel->second, CHANEL_OPERATOR) == 0)
			{
				sendMessagetoClient(client, ERR_CHANOPRIVSNEED(client->getMyNickname(), current_chanel->getChanelName()));
				return (false);
			}
		}

		for (; chanel_begin != chanel_end; chanel_begin++)
			sendMessagetoClient(const_cast<Client*>(chanel_begin->first), KICK_CLIENT(client->getMyNickname(), client->getMyUserName(), cmd[0], cmd[1], current_client->getMyNickname()));

		// DELETE CLIENT IN THE CHANEL MAP
		current_chanel->deleteClient(current_client->getMyNickname());

		// DELETE CHANEL IN THE CLIENT MAP 
		current_client->deleteChanel(current_chanel->getChanelName());
		return (true);
	}
	return (false);
}

bool    Irc::topic(Client* client, std::vector<std::string> cmd)
{
	if (cmd.size() < 2)
	{
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), cmd[0]));  
		return (false);
	}

	Chanel	*current_chanel = findChanel(cmd[1]);
	// CHECK IF THE CHANEL EXIST
	if (current_chanel == NULL)
	{
		sendMessagetoClient(client, ERR_NOSUCHCHANNEL(client->getMyNickname(), cmd[1]));
		return (false);
	}

	Chanel::map_iterator	client_in_chanel = current_chanel->getClient(client->getMyNickname());
	Chanel::map_iterator	chanel_end = current_chanel->getclientMap().end();
	Chanel::map_iterator	chanel_begin = current_chanel->getclientMap().begin();
	
	// GET INFO OF THE TOPIC
	if (cmd.size() == 2)
	{
		if (current_chanel->getChanelTopic() == "")
			sendMessagetoClient(client, RPL_NOTOPIC(client->getMyNickname(), cmd[1]));
		else
			sendMessagetoClient(client, RPL_TOPIC(client->getMyNickname(), cmd[1], current_chanel->getChanelTopic()));
	}

	// EDIT THE CHANEL TOPIC
	else
	{
		// CHECK PRIVILEGE
		if (current_chanel->isChanelModeActivated(TOPIC_PROTECTION))
		{
			if (client->isServerModeActivated(SERVER_OPERATOR) == 0)
			{
				// IF YOUR ARE NOT SERVER OPERATOR YOU MUST BE IN THE CHANEL AND BE CHANEL OPERATOR
				if ((client_in_chanel == chanel_end) || current_chanel->isClientModeActivated(client_in_chanel->second, CHANEL_OPERATOR) == 0)
				{
					sendMessagetoClient(client, ERR_CHANOPRIVSNEED(client->getMyNickname(), current_chanel->getChanelName()));
					return (false);
				}
			}
		}
	
		// SET THE TOPIC CORRECTLY
		std::string topic;
		for (std::vector<std::string>::iterator it = cmd.begin() + 2; it != cmd.end(); it++) 
		{
			topic += *it;
			std::vector<std::string>::iterator cpy = it;
			if (++cpy != cmd.end())
				topic += " ";
		}
		current_chanel->setChanelTopic(topic);

		for (; chanel_begin != chanel_end; chanel_begin++)
		{
			sendMessagetoClient(const_cast<Client*>(chanel_begin->first), SET_TOPIC(client->getMyNickname(), client->getMyUserName(), cmd[0], cmd[1], topic));
		}

	}
	return (true);
}

bool    Irc::notice(Client *client, std::vector<std::string> msg) 
{
	for (client_it it = _client.begin(), ite = _client.end(); it != ite; it++)
	{
		if ((*it)->getMyNickname() == msg[1])
		{
			sendMessagetoClient(*it, NOTICE(client->getMyNickname(), client->getMyUserName(), msg[0], (*it)->getMyNickname(), msg[2]));
			return (true);
		}
	}
	return (false);
}

bool	Irc::checkInfo(Client* client, std::vector<std::string> info) 
{
	if (info.size() < 8)
		sendMessagetoClient(client, ERR_NEEDMOREPARAMS(client->getMyNickname(), "CONNECT"));  
	else if (info[0] != "CAP") {
		std::cout << "value : [ " << info[0] << "]" << std::endl;
		sendMessagetoClient(client, ERR_UNKNOWNCOMMAND(client->getMyNickname(), info[0]));
	}
	else if (info[1] != "LS" ) 
		sendMessagetoClient(client, ERR_UNKNOWNCOMMAND(client->getMyNickname(), info[1]));
	else if (info[2] != "PASS") 
		sendMessagetoClient(client, ERR_UNKNOWNCOMMAND(client->getMyNickname(), info[2]));
	else if (info[4] != "NICK") 
		sendMessagetoClient(client, ERR_UNKNOWNCOMMAND(client->getMyNickname(), info[4]));
	else if (info[6] != "USER") 
		sendMessagetoClient(client, ERR_UNKNOWNCOMMAND(client->getMyNickname(), info[6]));
	else
		return (true);
	return (false);
}