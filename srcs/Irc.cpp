/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 18:06:38 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/13 20:00:40 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Utils.hpp"


bool Irc::msg(int client_fd, std::vector<std::string> split_cmd)
{
    (void)client_fd;
    (void)split_cmd;
    return (1);
}
bool Irc::join(int client_fd, std::vector<std::string> split_cmd)
{
    (void)client_fd;
    (void)split_cmd;
    return (1);
}
bool Irc::leave(int client_fd, std::vector<std::string> split_cmd)
{
    (void)client_fd;
    (void)split_cmd;
    return (1);
}
bool Irc::list(int client_fd, std::vector<std::string> split_cmd)
{
    (void)client_fd;
    (void)split_cmd;
    return (1);
}
bool Irc::nick(int client_fd, std::vector<std::string> split_cmd)
{
    (void)client_fd;
    (void)split_cmd;
    return (1);
}
bool Irc::quit(int client_fd, std::vector<std::string> split_cmd)
{
    
    if (split_cmd.size() > 1)
    {
        std::cout << RED << "error: " << RESET << "Usage: /quit" << std::endl;
        return (0);
    }
    Irc::eraseClient(0, client_fd);
    return (1);
}
bool Irc::who(int client_fd, std::vector<std::string> split_cmd)
{
    (void)client_fd;
    (void)split_cmd;
    return (1);
}
