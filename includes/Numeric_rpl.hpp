/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Numeric_rpl.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/30 16:36:53 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/30 19:14:42 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __NUMERIC_RPL_HPP__
# define  __NUMERIC_RPL_HPP__

# define user_id(nickname, username) (":" + nickname + "!" + username + "@localhost")

// #001
# define RPL_WELCOME(nickname) (":localhost 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "\r\n")

// #002
# define RPL_YOURHOST(client, server) (":localhost 002 " + client + " :Your host is " + server + ", running version 0.1\r\n")

// #003
# define RPL_CREATED(client, server) (":localhost 003 " + client + " :This server was created " + server + "\r\n")

// #004
# define RPL_MYINFO(client, server, version, available) (":localhost 004 " + client + " " + server + " " + version + " " + available + "\r\n")


// PASS
# define ERR_PASSWDMISMATCH(client) (":localhost 464 " + client + " :Password incorrect.\r\n")

// NICK 
# define RPL_NICK(oclient, uclient, client) (":" + oclient + "!" + uclient + "@localhost NICK " +  client + "\r\n")

#endif