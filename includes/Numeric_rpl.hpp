/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Numeric_rpl.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/30 16:36:53 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/04 18:52:46 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __NUMERIC_RPL_HPP__
# define  __NUMERIC_RPL_HPP__

# define user_id(nickname, username, cmd) (":" + nickname + "!~" + username + "@localhost " + cmd + " ")

            /* = = =    TO CONNECT    = = = */
// #001
# define RPL_WELCOME(nickname) (":localhost 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "\r\n")
// #002
# define RPL_YOURHOST(nickname, version) (":localhost 002 " + nickname + " :Your host is localhost, " + "running version " + version + "\r\n")
// #003
# define RPL_CREATED(nickname, date) (":localhost 003 " + nickname + " :This server was created " + date + "\r\n")
// #004
# define RPL_MYINFO(nickname, version) (":localhost 004 " + nickname + " localhost " + version + "\r\n")

            /* = = =    MODE    = = = */
# define RPL_MODE(nickname, username, cmd, mode) (user_id(nickname, username, cmd) + nickname + " :+" + mode + "\r\n")

            /* = = =    PASS    = = = */
// 464
# define ERR_PASSWDMISMATCH(nickname) (":localhost 464 " + nickname + " :Password incorrect.\r\n")

            /* = = =    WHO     = = = */
// 311
# define RPL_WHOISUSER(nickname, other_nick, username) (":localhost 311 " + nickname + " " + other_nick + "~" + username + " localhost * :" + username + "\r\n")
// 312
# define RPL_WHOISSERVER(nickname, other_nick) (":localhost 312 " + nickname + " " + other_nick + " localhost :<insert catchy phrase here>\r\n")
// 317
# define RPL_WHOISIDLE(nickname, other_nick, create_time) (":localhost 317 " + nickname + " " + other_nick + " " + create_time + " :second idle, signon time\r\n")
// 318
# define RPL_ENDOFWHOIS(nickname, other_nick) (":localhost 318 " + nickname + " " + other_nick + " :End of /WHOIS list.\r\n")
// 369
# define RPL_ENDOFWHOWAS(nickname, other_nick) (":localhost 369 " + nickname + " " + other_nick + " :End of WHOWAS\r\n")

            /* = = =    LIST    = = = */
// 321
# define RPL_LISTSTART(nickname) (":localhost 321 " + nickname + " Channel :Users Name\r\n")
// 322
# define RPL_LIST(nickname) (":localhost 322 " + nickname + " " /* == + channelname + " " + usercount == */+ "#test 1 :\r\n")
// 323
# define RPL_LISTEND(nickname) (":localhost 323 " + nickname + " :End of /LIST\r\n")

            /* = = =    NICK    = = = */
// 401
# define ERR_NOSUCHNICK(nickname, other_nick) (":localhost 401 " + nickname + " " + other_nick + " :No such nick\r\n")
// 406
# define ERR_WASNOSUCHNICK(nickname, other_nick) (":localhost 406 " + nickname + " " + other_nick + " :There was no such nickname\r\n")
// 433
# define ERR_NICKNAMEINUSE(nickname) (":localhost 433 * " + nickname + " :Nickname is already in use\r\n")

            /* = = =    GENERIC ERR     = = = */
// 461
# define ERR_NEEDMOREPARAMS(nickname, cmd) (":localhost 461 " + nickname + " " + cmd + ":Not enough parameters\r\n")

            /* = = =    PING / PONG     = = = */
// 513
# define ERR_BADPING(nickname) (":localhost 513 " + nickname + " has'nt send a right PONG\r\n")
# define RPL_PONG ":localhost PONG localhost :localhost\r\n"
# define CMD_PING(token) ("PING :" + token + "\r\n")






#endif