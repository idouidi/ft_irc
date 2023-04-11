/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:17:22 by asimon            #+#    #+#             */
/*   Updated: 2023/04/11 14:39:16 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANEL_HPP__
# define __CHANEL_HPP__
# include "Control.hpp"

class Chanel {
	
	public:
		typedef std::map<Client*, std::vector<client_mode> > 	client_map;
		typedef client_map::iterator 							map_iterator;
		
		Chanel(std::string&);
		~Chanel();



		bool						addClient(Client* client_to_add, std::vector<client_mode> mode_client);
		bool						deleteClient(std::string name);

		bool						isPresentInList(std::vector<std::string>& list, std::string name);
		bool						isPresentInChanel(std::string name);
		bool						isValidMode(char mode, chanel_mode &idx);
		bool						isModeActivated(chanel_mode mode);

	
		std::string					getChanelName() const;
		size_t						getNumClient() const;
		std::vector<std::string>&	getBlackList();
		std::vector<std::string>&	getWhiteList();
		std::vector<chanel_mode>&	getActiveModes() ;
		client_map&					getclientMap();
		// map_iterator				getClient(std::string name);

		bool						setModes(char mode);
		bool						unsetModes(char mode);
		
		void						setChanelName(std::string name);

		std::string 				listChanelModes();
		std::string					listClientmodes(std::vector<client_mode>& client_mode_in_chanel);
		std::string 				listAllClientsModesAndNames();

		template <typename T2>
		bool						operator==(T2& rhs) const { return (*this == rhs); }

		template <typename T1>
		bool						operator<(T1& rhs) const { return (_id < rhs._id); }

	private:
		static size_t							_id_global;
		size_t									_id;
		std::string								_name;
		std::string								_topic;
		std::vector<std::string>				_black_list;
		std::vector<std::string>				_white_list;
		std::vector<chanel_mode>				_active_modes; // mode in chanel
		client_map								_clients_in;
		std::string								_key;
};

#endif
