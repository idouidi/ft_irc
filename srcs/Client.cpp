/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asimon <asimon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 15:15:37 by asimon            #+#    #+#             */
/*   Updated: 2023/04/07 14:19:21 by asimon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

Client::Client(int socket, std::string token): my_socket(socket), token_ping(token), new_client(1), last_active_time(time(0)), current_chanel(0x0)
{}


Client::~Client() {}

Client::Client(const Client& c):my_socket(c.my_socket), token_ping(c.token_ping), nickname(c.nickname), new_client(c.new_client), 
last_active_time(c.last_active_time)
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
		last_active_time =  c.last_active_time;
		if (!c.my_chanels.empty())
		{
			my_chanels.clear();
			my_chanels.insert(c.my_chanels.begin(), c.my_chanels.end());
		}
	}
	return (*this);
}

////////////////////////////////////////////////////////////////////////////////


int							Client::getMySocket() const { return (my_socket); }

time_t 						Client::getLastActiveTime() const { return (last_active_time); }

std::string 				Client::getMyNickname() const { return (nickname); }

std::string 				Client::getMyUserName() const { return (username); }

void						Client::getMyUserMode() const {}

std::string					Client::getToken() const { return (token_ping); }

std::vector<client_mode>& 	Client::getActiveModes()  { return (active_modes); }

Client::chanel_map& 		Client::getChanelMap() {return (my_chanels); }


////////////////////////////////////////////////////////////////////////////////


void 				Client::setStatusClient(bool status) { new_client = status; }

void 				Client::setNickName(std::string nick) { nickname.assign(nick); }

void 				Client::setUserName(std::string user) { username.assign(user); }

void				Client::setCurrentChanel(Chanel& current) {
	current_chanel = &current;
	return ;
}

void				Client::setLastActiveTime() { last_active_time = time(0); }

bool				Client::setModes(char mode)
{
	client_mode idx;

	if (isValidMode(mode, idx) == 0)
		return (false);
	for (std::size_t i = 0; i < active_modes.size(); i++)
		if (idx == active_modes[i])
			return (false);
	active_modes.push_back(idx);
	return (true);
}

bool				Client::unsetModes(char mode)
{
	client_mode idx;

	if (isValidMode(mode, idx) == 0)
		return (false);

	for (std::size_t i = 0; i < active_modes.size(); i++)
	{
		if (idx == active_modes[i])
		{
			active_modes.erase(active_modes.begin() + i);
			return (true);
		}
	}
	return (false);
}

bool				Client::insertChanel(Chanel& chanel_to_add, std::vector<chanel_mode> chan_mode) 
{
	return (my_chanels.insert(std::pair<Chanel, std::vector<chanel_mode> >(chanel_to_add, chan_mode)).second);
}

////////////////////////////////////////////////////////////////////////////////


bool 				Client::isNewClient() const  { return (new_client); }


bool				Client::isValidMode(char mode, client_mode& idx)
{
    switch (mode)
    {
        case 'o':
			idx = OPERATOR;
            return (true);
        case 'v':
			idx = VOICE;
            return (true);
        case 'h':
			idx = HALF_OP;
            return (true);
        case 'a':
			idx = ADMIN;
            return (true);
        case 'i':
			idx = INVISIBLE;
            return (true);
        case 'w':
			idx = WALLOPS;
            return (true);
        default:
			idx = NON_CLIENT_MODE;
            return (false);
    }
}
