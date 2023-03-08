/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:04:28 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/08 12:31:09 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FT_IRC_HPP__
#define __FT_IRC_HPP__

# include <iostream>		// for stl
# include <unistd.h>		// for read / close 
# include <cstdlib>			// for exit
# include <cstdio>			// for perror
# include <cstring>         // for memset
# include <sys/epoll.h>		// for epoll
# include <netinet/in.h>    //for htons
# include <vector>
# include <stack>
# include <map>
// #include <sys/socket.h>

# include "client.hpp"

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
# define BUFFER_SIZE        1024




class Irc
{
		
	private:

}


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