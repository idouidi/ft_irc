/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asimon <asimon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:21:44 by asimon            #+#    #+#             */
/*   Updated: 2023/04/05 17:22:09 by asimon           ###   ########.fr       */
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
	for (map_iterator it = _clients_in.begin(), ite = _clients_in.end(); it != ite; it++) {
		if (it->first == const_cast<const Client&>(client_to_delete)) {
			_clients_in.erase(it);
			return (true);
		}
	}
	return (false);
}

bool		Chanel::addClient(Client& client_to_add, client_mode mode_to_give) {
	return (_clients_in.insert(std::pair<Client, client_mode>(client_to_add, mode_to_give)).second);
}

size_t		Chanel::getNumClient() const {
	return (_clients_in.size());
}