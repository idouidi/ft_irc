/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:04:28 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/13 15:09:13 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IRC_HPP__
#define __IRC_HPP__

# include "Utils.hpp"

class Irc
{
	public:
		// CONSTRUCTOR
		Irc(std::string port)
		{
			if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
			{
				perror("socket");
				throw std::runtime_error("Error when creating the socket");
			}
			epoll_fd = -1;
			memset(&event, 0, sizeof(event));
			server_address.sin_family = AF_INET;
			server_address.sin_addr.s_addr = INADDR_ANY;
			server_address.sin_port = htons(std::atoi(port.c_str()));
			memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));

		}

		// GETTER
		int getServerFd() const { return (server_fd); }
		int getEpollFd() const { return (epoll_fd); }
		struct epoll_event getEvent(int i) const { return (events[i]); } 
		struct epoll_event* getEventTab() { return (events); } 


		// MEMBER FUNCTIONS
		void init_server()
		{
			if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0 )
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
			std::cout << GREEN << "Client connected !" << RESET << std::endl;
		}

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
			close(client_fd);
			std::cout << RED << "Client deconnected !" << RESET << std::endl;

		}
		
		void closeServer()
		{
			while(!_client.empty())
				_client.erase(_client.begin());

			while(!_chanel.empty())
				_chanel.erase(_chanel.begin());
			close(server_fd);
		}

	private:
		int										server_fd;
		int										epoll_fd;
		struct epoll_event						event;
		struct epoll_event						events[MAX_EVENTS];
		struct sockaddr_in 						server_address;
		std::vector<int> 						_chanel;
		std::vector<Client>						_client;


};


#endif

