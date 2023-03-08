/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:04:28 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/08 16:42:09 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IRC_HPP__
#define __IRC_HPP__

# include "Utils.hpp"

class Irc
{
	public:
		Irc(std::string port)
		{
			if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
			{
				perror("socket");
				throw std::runtime_error("Error when creating the socket");
			}
			epoll_fd = -1;
			memset(&event, 0, sizeof(event));
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = INADDR_ANY;
			address.sin_port = htons(std::atoi(port.c_str()));
			memset(address.sin_zero, '\0', sizeof(address.sin_zero));

		}

		void init_server()
		{
			if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0 )
			{
				perror("bind");
				throw std::runtime_error("Error when linking the socket to the port");
			}
			if (listen(server_fd, MAX_CLIENT) < 0)
    		{
    	    	perror("listen");
    	    	throw std::runtime_error("Error when listening to the socket");
    		}
			if ((epoll_fd = epoll_create(MAX_CLIENT)) == -1 )
			{
				perror("epoll_create");
				throw std::runtime_error("Error while creating epoll_fd");
			}
			event.data.fd = server_fd;
			event.events = EPOLLIN; //data can be read from a fd (server_fd) monitored by epoll 
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1 )
			{
				perror("epoll_ctl EPOLL_CTL_ADD");
				throw std::runtime_error("Error while adding the server socket to epoll");
			}
		}

		int AdressSize() const { return (sizeof(address)); }
		int getServerFd() const { return (server_fd); }
		int getEpollFd() const { return (epoll_fd); }


		

	private:
		int										server_fd;
		int										epoll_fd;
		struct epoll_event						event;
		struct sockaddr_in 						address;
		std::stack<int, std::vector<int> > 		_chanel;
		std::stack<Client, std::vector<int> >	_client;


};


#endif

