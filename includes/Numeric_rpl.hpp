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

# define user_id(nickname, username) (":" + nickname + "!~" + username + "@localhost")

// #001
# define RPL_WELCOME(nickname) (":localhost 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "\r\n")

// #002
# define RPL_YOURHOST(nickname, version) (":localhost 002 " + nickname + " :Your host is localhost, " + "running version " + version + "\r\n")

// #003
# define RPL_CREATED(nickname, date) (":localhost 003 " + nickname + " :This server was created " + date + "\r\n")

// #004
# define RPL_MYINFO(nickname, version) (":localhost 004 " + nickname + " localhost " + version + "\r\n")

// MODE
# define RPL_MODE(nickname, username,  mode) (user_id(nickname, username) + " MODE " + nickname + " :+" + mode)

//PING
# define RPL_PING ":localhost PONG localhost :localhost"

// PASS
# define ERR_PASSWDMISMATCH(nickname) (":localhost 464 " + nickname + " :Password incorrect.\r\n")

// MISS PARAMS
# define ERR_NEEDMOREPARAMS(nickname, cmd) (":localhost 461 " + nickname + " " + cmd + ":Not enough parameters\r\n")


/*
*  NICK
*/
# define ERR_NOSUCHNICK(nickname, oher_nickname) (":localhost 401 " + nickname + " " + other_nickname + ":No such nick\r\n")
# define ERR_WASNOSUCHNICK(nickname, other_nickname) (":localhost 406 " + nickname + " " + other_nickname + " :There was no such nickname\r\n")
# define ERR_NICKNAMEINUSE(nickname) (":localhost 433 * " + nickname + ":Nickname is already in use\r\n")


/*
*  WHO
*/
# define RPL_WHOISUSER(nickname, username) (":localhost 311 " + nickname + " " + nickname + "~" + username + " localhost * :" + username + "\r\n")
# define RPL_WHOISSERVER(nickname) (":localhost 312 " + nickname + " " + nickname + " localhost :<insert catchy phrase here>\r\n")
# define RPL_WHOISIDLE(nickname, create_time) (":localhost 317 " + nickname + " " + nickname + " " + create_time + " :second idle, signon time\r\n")
# define RPL_ENDOFWHOIS(ninckname) (":localhost 318 " + nickname + " " + nickname + " :End of /WHOIS list.\r\n")
# define RPL_ENDOFWHOWAS(nickname, other_nickname) (":localhost 369 " + nickname + " " + other_nickname + " :End of WHOWAS\r\n")

#endif