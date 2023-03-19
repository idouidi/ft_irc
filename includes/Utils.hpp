/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 12:55:15 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/19 17:25:12 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <iostream>		// for stl
# include <unistd.h>		// for read / close 
# include <cstdlib>			// for exit
# include <cstdio>			// for perror
# include <cstring>         // for memset
# include <sys/epoll.h>		// for epoll
# include <netinet/in.h>    // for htons
# include <signal.h>        // for signal
# include <sstream>         // for string stream
# include <vector>
# include <map>
# include <exception>
// #include <sys/socket.h>


# define RESET	            "\033[0m"
# define BLACK	            "\033[1;30m"
# define RED	            "\033[1;31m"
# define GREEN	            "\033[1;32m"
# define YELLOW             "\033[1;33m"
# define MAGENTA            "\033[1;35m"
# define CYAN	            "\033[1;36m"

# define OK                 "\033[1;32m[OK]\033[0m"
# define KO                 "\033[1;31m[KO]\033[0m"

# define MAX_CLIENT   		10
# define MAX_EVENTS         10
# define BUFFER_SIZE        1024

# include "Client.hpp"
# include "Irc.hpp"

#endif


/* **********************/
/* FUNCTION ALLOWS		*/
/*	socket				*/
/* 	setsockopt			*/
/* 	getsockname			*/
/* 	getprotobyname		*/
/*	gethostbyname		*/
/* 	getaddrinfo			*/
/* 	freeaddrinfo		*/
/* 	bind				*/
/*	connect				*/
/* 	listen				*/
/* 	accept				*/
/* 	htons				*/
/* 	htonl				*/
/* 	ntohs				*/
/*	ntohl				*/
/* 	inet_addr			*/
/* 	inet_ntoa			*/
/* 	send				*/
/* 	recv				*/
/* 	signal				*/
/*	lseek				*/
/* 	fstat				*/
/* 	fcntl				*/
/* 	poll 				*/
/*	epoll				*/
/*	kqueue				*/
/*	select				*/
/* **********************/ 