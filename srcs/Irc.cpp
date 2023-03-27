/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 18:06:38 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/27 19:18:12 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Utils.hpp"


bool Irc::msg(Client& client, std::vector<std::string> cmd)
{
    std::string msg;
    if (cmd.size() < 3)
    {
        msg = std::string(RED) + "Error: " + std::string(RESET) + "Invalid format\n";
        return (false);
    }
    for (size_t i = 0; i < _client.size(); i++)
    {
        if (cmd[1] == _client[i].getMyNickname())
        {
            if (client.getMyNickname() == _client[i].getMyNickname())
			{
				msg = std::string(RED) + "Error: " +  std::string(RESET) + "You can't send a message to yourself!";
				sendMessagetoClient(_client[i].getMySocket(), msg);
				return (false);
			}
            msg = std::string(YELLOW) + "[" + std::string(CYAN) + client.getMyNickname() + std::string(YELLOW ) + "]"\
			+ std::string(CYAN) + " >" + std::string(RESET);
            sendMessagetoClient(_client[i].getMySocket(), msg);

            for (size_t j = 2 ; j < cmd.size(); j++ )
                sendMessagetoClient(_client[i].getMySocket(), " " + std::string(YELLOW) + cmd[j] + std::string(RESET));

            sendMessagetoClient(_client[i].getMySocket(), "\n");
            return (true);
        }
    }
	msg = std::string(RED) + "Error: " +  std::string(RESET) +  std::string(YELLOW) + "["\
	+ std::string(CYAN) + cmd[1] + std::string(YELLOW ) + "]" + std::string(RESET) +\
	" is not referenced on the server.";
	sendMessagetoClient(client.getMySocket(), msg);
    return (0);
}

bool Irc::join(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    (void)cmd;
    return (1);
}
bool Irc::leave(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    (void)cmd;
    return (1);
}
bool Irc::list(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    (void)cmd;
    return (1);
}
bool Irc::nick(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    (void)cmd;
    return (1);
}
bool Irc::quit(Client& client, std::vector<std::string> cmd)
{
    
    if (cmd.size() > 1)
    {
        std::cout << RED << "error: " << RESET << "Usage: /quit" << std::endl;
        return (0);
    }
    Irc::eraseClient(client);
    return (1);
}
bool Irc::who(Client& client, std::vector<std::string> cmd)
{
    (void)client;
    (void)cmd;
    return (1);
}