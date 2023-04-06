/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:17:22 by asimon            #+#    #+#             */
/*   Updated: 2023/04/06 14:04:31 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANEL_HPP__
# define __CHANEL_HPP__
# include "Control.hpp"

class Chanel {
	
	public:
		enum									client_mode { I, S, W, O, P, T, N, M, L, B, K, V };
		typedef std::map<Client, client_mode> 	client_map;
		typedef client_map::iterator 			map_iterator;
		
		Chanel(std::string, size_t);
		~Chanel();


		std::string	getChanelName();

		bool						addClient(Client& client_to_add, client_mode chan_mode);
		bool						deleteClient(Client& client_to_delete);

		bool						isPresentInList(std::vector<std::string>& list, std::string name);
		bool						isPresentInChanel(std::string name);

		size_t						getNumClient() const;
		std::vector<std::string>&	getBlackList();
		std::vector<std::string>&	getWhiteList();
		client_map&					getclientMap();
		std::string list_client();


		template <typename T1>
		bool		operator<(T1& rhs) const { return (_id < rhs._id); }
		
		
	private:
		size_t									_id;
		std::string								_name;
		std::string								_topic;
		std::vector<std::string>				black_list;
		std::vector<std::string>				white_list;
		client_map								_clients_in;
};

#endif
