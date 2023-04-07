/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asimon <asimon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:17:22 by asimon            #+#    #+#             */
/*   Updated: 2023/04/07 14:13:56 by asimon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANEL_HPP__
# define __CHANEL_HPP__
# include "Control.hpp"

class Chanel {
	
	public:
		typedef std::map<Client, std::vector<client_mode_e> > 	client_map;
		typedef client_map::iterator 							map_iterator;
		
		Chanel(std::string&);
		~Chanel();


		std::string	getChanelName();

		bool						addClient(Client& client_to_add, std::vector<client_mode_e> mode_client);
		bool						deleteClient(Client& client_to_delete);
		bool						isPresentInList(std::vector<std::string>& list, std::string name);
		bool						isPresentInChanel(std::string name);
		bool						isValidMode(char mode, chanel_mode_e &idx);
		
		size_t						getNumClient() const;
		std::vector<std::string>&	getBlackList();
		std::vector<std::string>&	getWhiteList();
		std::vector<chanel_mode_e>&	getActiveModes() ;
		client_map&					getclientMap();
		void						setModes(char mode);

		std::string 				listClients();
		std::string 				listModes();

		template <typename T1>
		bool						operator<(T1& rhs) const { return (_id < rhs._id); }

	private:
		static size_t							_id_global;
		size_t									_id;
		std::string								_name;
		std::string								_topic;
		std::vector<std::string>				_black_list;
		std::vector<std::string>				_white_list;
		std::vector<chanel_mode_e>				_active_modes; // mode in chanel
		client_map								_clients_in;
		std::string								_key;
};

#endif
