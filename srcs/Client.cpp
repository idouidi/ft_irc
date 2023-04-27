/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 15:15:37 by asimon            #+#    #+#             */
/*   Updated: 2023/04/27 17:19:37 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

Client::Client(int socket, std::string token): my_socket(socket), token_ping(token), new_client(1), last_active_time(time(0)), init(1), cmd_line(std::string())
{}

Client::~Client() {}

Client::Client(const Client& c):my_socket(c.my_socket), token_ping(c.token_ping), nickname(c.nickname), new_client(c.new_client), 
last_active_time(c.last_active_time), init(1), cmd_line(std::string())
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
	{
		my_socket = c.my_socket;
		token_ping = c.token_ping;
		nickname = c.nickname;
		new_client = c.new_client;
		last_active_time =  c.last_active_time;
		init = c.init;
		cmd_line = c.cmd_line;
		if (!c.my_chanels.empty())
		{
			my_chanels.clear();
			my_chanels.insert(c.my_chanels.begin(), c.my_chanels.end());
		}
	}
	return (*this);
}

int							Client::getMySocket() const { return (my_socket); }

time_t 						Client::getLastActiveTime() const { return (last_active_time); }

std::string 				Client::getMyNickname() const { return (nickname); }

std::string 				Client::getMyUserName() const { return (username); }

std::string					Client::getToken() const { return (token_ping); }

std::string&				Client::getCmdLine() { return (cmd_line); }

bool						Client::getInitStatus() { return (init); }

std::vector<client_mode>& 	Client::getActiveModes()  { return (active_modes); }

Client::chanel_map& 		Client::getChanelMap() {return (my_chanels); }

void 						Client::setStatusClient(bool status) { new_client = status; }

void 						Client::setNickName(std::string nick) { nickname.assign(nick); }

void 						Client::setUserName(std::string user) { username.assign(user); }

void						Client::setLastActiveTime() { last_active_time = time(0); }

void						Client::setInitStatus(bool status) { init = status; }

bool 						Client::isNewClient() const  { return (new_client); }

void						Client::setCmdLine(std::string new_one) { cmd_line = new_one; }

bool						Client::isValidMode(char mode, client_mode& idx)
{
    switch (mode)
    {
		case 'O':
			idx = SERVER_OPERATOR;
			return (true);
        case 'o':
			idx = CHANEL_OPERATOR;
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

bool						Client::isServerModeActivated(client_mode mode)
{
	for (std::size_t i = 0; i < active_modes.size(); i++)
	{
		if (mode == active_modes[i])
			return (true);
	}
	return (false);
	
}

bool						Client::setServerModes(char mode)
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

bool						Client::unsetServerModes(char mode)
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

bool						Client::insertChanel(Chanel* chanel_to_add, std::vector<client_mode> chan_mode) 
{
	my_chanels[chanel_to_add] = chan_mode;
	return (true);
}

bool						Client::deleteChanel(std::string name)
{
	for (map_iterator it = my_chanels.begin(), ite = my_chanels.end( ); it != ite; it++)
	{
		if (it->first->getChanelName() == name)
		{
			my_chanels.erase(it);
			return (true);
		}
	}
	return (false);
}