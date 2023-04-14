/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Control.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 12:55:15 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/14 13:23:28 by idouidi          ###   ########.fr       */
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
# include <cstddef>         // for nullptr
# include <fcntl.h>
# include <ctime>
// #include <sys/socket.h>

/*
*   /RMRECONNS  Irssi: Removed reconnection to server localhost port XXXX
    to avoid irssi to retest this port when an error appear
*/


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

/*
*   - +o : (operateur)
*
*   - +v : (voice) pour donner la permission de parler sur un canal modéré
*
*   - +h : (half-op) pour donner des privilèges supplémentaires à un utilisateur sur un canal
*
*   - +a : (admin) pour donner des privilèges administratifs à un utilisateur
*          permet à un utilisateur de devenir automatiquement opérateur dès qu'il se connecte au canal.
*
*   - +i : permet de rendre l'utilisateur invisible (son nom n'apparaîtra pas dans la liste des membres du salon)
*
*   - +w : permet aux utilisateurs ayant ce mode activé de recevoir certains types de messages dits "importants", 
*          tels que les erreurs de serveur, les déconnexions et les reconnexions d'utilisateurs importants, etc.
*/
enum client_mode {SERVER_OPERATOR, CHANEL_OPERATOR, VOICE, HALF_OP, ADMIN, INVISIBLE, WALLOPS, NON_CLIENT_MODE}; 

/*
*   - +t : canal protégé, seuls les opérateurs peuvent changer le sujet
*
*   - +n : pas de messages privés depuis le canal
*
*   - +s : canal secret, n'apparaît pas dans la liste des canaux
*
*   - +m : modéré, seuls les utilisateurs avec le mode voice ou supérieur peuvent parler
*
*   - +i : canal invite-only, seuls les utilisateurs invités peuvent rejoindre le canal
*
*   - +p : canal privé, n'apparaît pas dans la liste des canaux publics
*
*   - +k : canal avec mot de passe, les utilisateurs doivent fournir le mot de passe pour rejoindre le canal
*
*   - +l : limite d'utilisateurs sur le canal, empêche les utilisateurs supplémentaires de rejoindre une fois que la limite est atteinte
*
*/
enum chanel_mode {TOPIC_PROTECTION, NO_EXTERNAL_MSG, SECRET, MODERATED, INVITE_ONLY, PRIVATE, KEY, LIMIT, NON_CHANEL_MODE} ;

class Client;
class Chanel;
class Irc;

void                        printInServer(std::string msg, Client& client);
bool                        check_port(std::string port);
bool                        check_pswd(std::string pswd);
std::string                 getDateTime();
std::vector<std::string>    split(std::string str, const std::string charset);
char                        convertClientModeToChar(client_mode mode);
char                        convertChanelModeToChar(chanel_mode mode);


# include "Numeric_rpl.hpp"
# include "Client.hpp"
# include "Chanel.hpp"
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