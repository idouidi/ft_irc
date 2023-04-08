/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:21:44 by asimon            #+#    #+#             */
/*   Updated: 2023/04/07 16:54:30 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

size_t	Chanel::_id_global = 0;

Chanel::Chanel(std::string& name):  _id(_id_global++), _name(name) 
{}

Chanel::~Chanel() 
{}

std::string					Chanel::getChanelName() { return (_name); }

std::vector<std::string>&	Chanel::getBlackList() { return (_black_list); }

std::vector<std::string>&	Chanel::getWhiteList() { return (_white_list); }

Chanel::client_map&			Chanel::getclientMap() { return (_clients_in); }

Chanel::map_iterator Chanel::getClient(std::string name)
{
    map_iterator it = _clients_in.begin();
    map_iterator ite = _clients_in.end();
    for (; it != ite; it++)
    {
        if (name == it->first.getMyNickname())
            return it;
    }
    return ite;
}


std::vector<chanel_mode>&	Chanel::getActiveModes() { return (_active_modes); }

bool						Chanel::addClient(Client& client_to_add, std::vector<client_mode> mode_to_give) 
{
	_clients_in.insert(std::make_pair(client_to_add, mode_to_give));
	return true;
}

bool						Chanel::deleteClient(Client& client_to_delete) 
{
	for (map_iterator it = _clients_in.begin(), ite = _clients_in.end(); it != ite; it++) 
	{
		if (it->first == const_cast<const Client&>(client_to_delete)) 
		{
			_clients_in.erase(it);
			return (true);
		}
	}
	return (false);
}

bool						Chanel::isPresentInList(std::vector<std::string>& list, std::string name)
{
   for (size_t i = 0; i < list.size(); i++)
        if (list[i] == name)
            return (1);
    return (0);
}

bool						Chanel::isPresentInChanel(std::string client_name)
{
	map_iterator it = _clients_in.begin();
	map_iterator ite = _clients_in.end();
	for (; it != ite; it++)
		if (it->first.getMyNickname() == client_name)
			return (1);
	return (0);
}

size_t						Chanel::getNumClient() const 
{
	return (_clients_in.size());
}

std::string					Chanel::listClientmodes(map_iterator it)
{
	std::string list;

	for(size_t i = 0; i < it->second.size(); i++)
	{
		if (it->second[i] == CHANEL_OPERATOR)
			list += '@';
			else if (it->second[i] == VOICE)
			list += '+';
	}
	return (list);
}

std::string					Chanel::listAllClientsModesAndNames()
{
	map_iterator it = _clients_in.begin();
	map_iterator ite = _clients_in.end();
	std::string list;
	std::string modes;

	for (; it != ite; it++)
	{
		modes = listClientmodes(it);
		std::string nick = it->first.getMyNickname();
		list += modes + nick;
		map_iterator cpy = it;
		if (++cpy != ite)
			list += " ";
	}
	return (list);
}

std::string				Chanel::listModes()
{
	std::string list;
	for (std::size_t i = 0; i < _active_modes.size(); i++)
		list += _active_modes[i];
	return (list);
}


bool				Chanel::isValidMode(char mode, chanel_mode& idx)
{
    switch (mode)
    {
        case 't':
			idx = TOPIC_PROTECTION;
            return (true);
        case 'n':
			idx = NO_EXTERNAL_MSG;
            return (true);
        case 's':
			idx = SECRET;
            return (true);
        case 'm':
			idx = MODERATED;
            return (true);
        case 'i':
			idx = INVITE_ONLY;
            return (true);
        case 'p':
			idx = PRIVATE;
            return (true);
        case 'k':
			idx = KEY;
            return (true);
        case 'l':
			idx = LIMIT;
            return (true);
        default:
            return (false);
    }
}

void				Chanel::setModes(char mode)
{
	chanel_mode idx;

	if (isValidMode(mode, idx) == 0)
		return ;

	for (std::size_t i = 0; i < _active_modes.size(); i++)
		if (idx == _active_modes[i])
			return ;
	_active_modes.push_back(idx);
}

void 			Chanel::setChanelName(std::string name) {_name = name; }