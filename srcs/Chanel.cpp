/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asimon <asimon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:21:44 by asimon            #+#    #+#             */
/*   Updated: 2023/04/05 18:07:12 by asimon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

Chanel::Chanel(std::string name, size_t id):  _id(id), _name(name) {
	return ;
}

Chanel::~Chanel() {
	return ;
}

bool		Chanel::deleteClient(Client& client_to_delete) {
	for (std::map<Client, mode_client>::iterator it = _clients_in.begin(), ite = _clients_in.end(); it != ite; it++) {
		if (it->first == const_cast<const Client&>(client_to_delete)) {
			_clients_in.erase(it);
			return (true);
		}
	}
	return (false);
}

bool		Chanel::addClient(Client& client_to_add, mode_client mode_to_give) {
	return (_clients_in.insert(std::pair<Client, mode_client>(client_to_add, mode_to_give)).second);
}

size_t		Chanel::getNumClient() const {
	return (_clients_in.size());
}