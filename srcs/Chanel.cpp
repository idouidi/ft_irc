/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:21:44 by asimon            #+#    #+#             */
/*   Updated: 2023/04/06 18:31:53 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

Chanel::Chanel(std::string name, size_t id):  _id(id), _name(name) 
{}

Chanel::~Chanel() 
{}

std::string					Chanel::getChanelName() { return (_name); }

std::vector<std::string>&	Chanel::getBlackList() { return (black_list); }

std::vector<std::string>&	Chanel::getWhiteList() { return (white_list); }

Chanel::client_map&			Chanel::getclientMap() {return (_clients_in); }

bool						Chanel::addClient(Client& client_to_add, client_mode mode_to_give) 
{
	return (_clients_in.insert(std::pair<Client, client_mode>(client_to_add, mode_to_give)).second);
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

std::string					Chanel::listClients()
{
	map_iterator it = _clients_in.begin();
	map_iterator ite = _clients_in.end();
	std::string list;
	std::string modes;

	for (; it != ite; it++)
	{
		for (std::size_t i = 0; i < it->second.size(); i++)
			modes += it->second[i]; 
		std::string nick = it->first.getMyNickname();
		list += modes + nick;
		if (it++ != ite)
			list += " ";
	}
	return (list);
}

std::string				Chanel::listModes()
{
	std::string list;
	for (std::size_t i = 0; i < active_modes.size(); i++)
		list += active_modes[i];
	return (list);
}

bool				Chanel::isValidMode(char mode, chanel_mode& idx)
{
    switch (mode)
    {
        case 't':
			idx = t;
            return (true);
        case 'n':
			idx = n;
            return (true);
        case 's':
			idx = s;
            return (true);
        case 'm':
			idx = m;
            return (true);
        case 'i':
			idx = i;
            return (true);
        case 'p':
			idx = p;
            return (true);
        case 'k':
			idx = k;
            return (true);
        case 'l':
			idx = l;
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

	for (std::size_t i = 0; i < active_modes.size(); i++)
		if (idx == active_modes[i])
			return ;
	active_modes.push_back(idx);
}