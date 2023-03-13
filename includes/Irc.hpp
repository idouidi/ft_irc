/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:04:28 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/13 20:03:33 by idouidi          ###   ########.fr       */
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

		Irc(std::string port)
		{
			if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
			{
				perror("socket");
				throw std::runtime_error("Error when creating the socket");
			}
			epoll_fd = -1;
			memset(&event, 0, sizeof(event));
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = INADDR_ANY;
			server_addr.sin_port = htons(std::atoi(port.c_str()));
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
		
		void addClient(int client_fd)
		{
			event.data.fd = client_fd;
			event.events = EPOLLIN;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1 )
			{
				perror("epoll_ctl EPOLL_CTL_ADD client soscket");
				exit(EXIT_FAILURE);
			}
			_client.push_back(Client(client_fd));
			std::cout << GREEN << "Client[ " << CYAN << client_fd << GREEN << " ] connected  !" << RESET << std::endl;
		}

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		void eraseClient(int event, int client_fd)
		{
			for (std::size_t i = 0; i < _client.size(); i++)
			{
				if (client_fd == _client[i].getMySocket())
					_client.erase(_client.begin() + i);
			}
			if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[event].data.fd, NULL ) == -1)
			{
						perror("epoll_ctl EPOLL_CTL_DEL");
						exit(EXIT_FAILURE);			
			}
			std::cout << RED << "Client[ " << CYAN << client_fd << RED << " ] deconnected !" << RESET << std::endl;
			close(client_fd);
		}

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/


		bool msg(int, std::vector<std::string>);
		bool join(int, std::vector<std::string>);
		bool leave(int, std::vector<std::string>);
		bool list(int, std::vector<std::string>);
		bool nick(int, std::vector<std::string>);
		bool quit(int, std::vector<std::string>);
		bool who(int, std::vector<std::string>);

		bool execCmd(int client_fd, std::string cmd)
		{
			bool (Irc::*tab[7])(int, std::vector<std::string> ) = {&Irc::msg, &Irc::join, &Irc::leave, &Irc::list, &Irc::nick, &Irc::quit, &Irc::who};
			std::string ref[] = {"/msg", "/join", "/leave", "/list", "/nick", "/quit", "/who" };
			std::vector<std::string> split;
			std::stringstream ss(cmd);
			std::string sub_string;
			
			while (std::getline(ss, sub_string, ' '))
				split.push_back(sub_string);

			if (split[0][0] != '/' && std::isalnum(split[0][1]))
			{
				std::cout << RED << "error: " << RESET << "Bad command format" << std::endl;
				return (0);
			}
			for (std::size_t i = 0; i < 7 ; i++)
			{
				if (ref[i] == split[0])
				{
					(this->*(tab[i]))(client_fd, split);
					break ;
				}
			}
			return (1);
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
		int										epoll_fd;
		struct epoll_event						event;
		struct epoll_event						events[MAX_EVENTS];
		struct sockaddr_in 						server_addr;
		std::vector<int> 						_chanel;
		std::vector<Client>						_client;
		
		Irc() {}


};


#endif

