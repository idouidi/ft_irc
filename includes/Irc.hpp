/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:04:28 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/27 00:34:54 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IRC_HPP__
#define __IRC_HPP__

# include "Utils.hpp"

class Irc
{
	public:
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		/*	
		*	📌 CONSTRUCTOR / DESTRCUTOR
		*/

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		Irc(char *port, char *pswd)
		{
			if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
			{
				perror("socket");
				throw std::runtime_error("Error when creating the socket");
			}
			server_pswd = pswd;
			epoll_fd = -1;
			memset(&event, 0, sizeof(event));
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = INADDR_ANY;
			server_addr.sin_port = htons(std::atoi(port));
			memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
		}

		~Irc() { closeServer(); }

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		/*	
		*	📌 GETTER
		*/

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		int getServerFd() const { return (server_fd); }
		
		int getEpollFd() const { return (epoll_fd); }
		
		struct epoll_event getEvent(int i) const { return (events[i]); } 
		
		struct epoll_event* getEventTab() { return (events); } 

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		/*	
		*	📌 MEMBER FUNCTION
		*/

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
	
		void init_server()
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

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		void sendMessagetoClient(int client_fd, std::string msg)
		{
			int			bytes_sent;
			int 		len = msg.size();

			if ((bytes_sent = send(client_fd, msg.c_str(), len, 0 )) != len)
			{
				perror("send failed");
				throw std::runtime_error("Error when sending a message to the client");
			}
		}

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		std::string recvMessageFromClient(int client_fd)
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

		bool goodPassword(Client& client, std::string pswd)
		{
			std::string msg;
			if (pswd != "\n")
			{
				pswd.erase(pswd.size() - 1);
				if (pswd == server_pswd)
				{
					msg = std::string(CYAN) +  "Welcome !\n" + std::string(RESET);
					sendMessagetoClient(client.getMySocket(), msg);
					msg = std::string(GREEN) + "ENTER YOUR NICKNAME: " + std::string(RESET);
					sendMessagetoClient(client.getMySocket(), msg);
					client.setStatusClient(false);
					return (1);

				}
			}
			msg  = std::string(RED) + "Error: " + std::string(RESET) + "Wrong password !\n";
			sendMessagetoClient(client.getMySocket(), msg);
			eraseClient(client);
			return (0);
		}

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
		Client* findClient(int fd_client)
		{
			for (size_t i = 0; i < _client.size(); i++)
			{
				if (_client[i].getMySocket() == fd_client)
					return (&_client[i]);
			}
			return NULL;
		}

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		void addClientNickname(Client& client, std::string nickname)
		{
			std::string msg;
			if (nickname == "\n")
				msg =  std::string(RED) + "Error: " +  std::string(RESET) + "nickname can't be empty !\n";
			else
			{
				nickname.erase(nickname.size() - 1);
				if (isNicknameAvaible(nickname))
				{
					client.setNickName(nickname);
					std::cout << GREEN << "Client[ " << CYAN << client.getMySocket() << GREEN << " ] is connected !" << RESET << std::endl;
					return ;
				}
				else
					msg = std::string(YELLOW) + "[" + std::string(RED) + nickname + std::string(YELLOW) + "] " + std::string(RESET) \
					+ "is already used by an another user :\\\n\n";

			}
			sendMessagetoClient(client.getMySocket(), msg);
			msg = std::string(GREEN) + "ENTER YOUR NICKNAME: " + std::string(RESET);
			sendMessagetoClient(client.getMySocket(), msg);
		}
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
		
		void addClient(int client_fd)
		{
			std::string msg = std::string(GREEN) + "ENTER THE SERVER PASSWORD: " + std::string(RESET);
			sendMessagetoClient(client_fd, msg);

			event.data.fd = client_fd;
			event.events = EPOLLIN;

			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1 )
			{
				perror("epoll_ctl EPOLL_CTL_ADD client soscket");
				exit(EXIT_FAILURE);
			}
			_client.push_back(Client(client_fd));
		}

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		void eraseClient(Client& client)
		{
			for (std::size_t i = 0; i < _client.size(); i++)
				if (client.getMySocket() == _client[i].getMySocket())
					_client.erase(_client.begin() + i);

			if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client.getMySocket(), NULL ) == -1)
			{
						perror("epoll_ctl EPOLL_CTL_DEL");
						exit(EXIT_FAILURE);			
			}
			std::cout << RED << "Client[ " << CYAN << client.getMySocket() << RED << " ] deconnected !" << RESET << std::endl;
			close(client.getMySocket());
		}

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		bool msg(Client&, std::vector<std::string>);
		bool join(Client&, std::vector<std::string>);
		bool leave(Client&, std::vector<std::string>);
		bool list(Client&, std::vector<std::string>);
		bool nick(Client&, std::vector<std::string>);
		bool quit(Client&, std::vector<std::string>);
		bool who(Client&, std::vector<std::string>);

		bool execCmd(Client& client , std::string cmd)
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
				std::string msg = std::string(RED) + "Error: " + std::string(RESET) + "Bad command format\n";
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

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
		
		void closeServer()
		{
			while(!_client.empty())
				_client.erase(_client.begin());

			while(!_chanel.empty())
				_chanel.erase(_chanel.begin());
			close(server_fd);
		}

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
	
	private:
		int										server_fd;
		std::string 							server_pswd;
		int										epoll_fd;
		struct epoll_event						event;
		struct epoll_event						events[MAX_EVENTS];
		struct sockaddr_in 						server_addr;
		std::vector<int> 						_chanel;
		std::vector<Client>						_client;

		Irc() {}

		bool isNicknameAvaible(std::string nickname)
		{
			if (nickname == "\n")
				return (0);
			for(size_t i = 0; i < _client.size(); i++)
				if (nickname == _client[i].getMyNickname())
					return (0);
			return (1);
		}
};


#endif

