/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asimon <asimon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 15:15:37 by asimon            #+#    #+#             */
/*   Updated: 2023/04/05 17:22:06 by asimon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

Client::Client(int socket, std::string token): my_socket(socket), token_ping(token), new_client(1), user_mode(0), last_active_time(time(0))
{}

Client::Client(const Client& c):my_socket(c.my_socket), token_ping(c.token_ping), nickname(c.nickname), new_client(c.new_client), 
user_mode(c.user_mode), last_active_time(c.last_active_time)
{
	if (!c.my_chanels.empty())
	{
		my_chanels.clear();
		my_chanels.insert(c.my_chanels.begin(), c.my_chanels.end());
	}
}

Client& Client::operator=(const Client& c)
{
	if (this != &c)
	{;
		my_socket = c.my_socket;
		token_ping = c.token_ping;
		nickname = c.nickname;
		new_client = c.new_client;
		user_mode = c.user_mode;
		last_active_time =  c.last_active_time;
		if (!c.my_chanels.empty())
		{
			my_chanels.clear();
			my_chanels.insert(c.my_chanels.begin(), c.my_chanels.end());
		}
	}
	return (*this);
}

Client::~Client() {}

int			Client::getMySocket() const { return (my_socket); }

time_t 		Client::getLastActiveTime() const { return (last_active_time); }

char 		Client::getMyUserMode() const {return (user_mode); }

std::string Client::getMyNickname() const { return (nickname); }

std::string Client::getMyUserName() const { return (username); }

std::string	Client::getToken() const { return (token_ping); }

void 		Client::setStatusClient(bool status) { new_client = status; }

void 		Client::setNickName(std::string nick) { nickname.assign(nick); }

void 		Client::setUserName(std::string user) { username.assign(user); }

void 		Client::setUserMode(char mode) { user_mode = mode; }

void		Client::setLastActiveTime() { last_active_time = time(0); }

bool 		Client::isNewClient() const  { return (new_client); }

bool 		Client::isUserModeUp() const { return (user_mode == 0); }

bool		Client::insertChanel(Chanel& chanel_to_add, chanel_mode chan_mode) 
{
	return (my_chanels.insert(std::pair<Chanel, chanel_mode>(chanel_to_add, chan_mode)).second);
}