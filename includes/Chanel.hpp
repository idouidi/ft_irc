/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:17:22 by asimon            #+#    #+#             */
/*   Updated: 2023/04/14 13:45:45 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANEL_HPP__
# define __CHANEL_HPP__
# include "Control.hpp"

class Chanel {
	
	public:
		typedef std::map<Client*, std::vector<client_mode> > 	client_map;
		typedef client_map::iterator 							map_iterator;
		typedef std::string 									(*ListFunc)(std::vector<client_mode>&);

		
		Chanel(std::string&);
		~Chanel();



		bool						addClient(Client* client_to_add, std::vector<client_mode> mode_client);
		bool						deleteClient(std::string name);

		bool						isPresentInList(std::vector<std::string>& list, std::string name);
		bool						isPresentInChanel(std::string name);
		bool						isValidMode(char mode, chanel_mode &idx);
		bool						isChanelModeActivated(chanel_mode mode);
		bool						isClientModeActivated(std::vector<client_mode>& modes, client_mode mode);

	
		std::string					getChanelName() const;
		size_t						getNumClient() const;
		std::vector<std::string>&	getBlackList();
		std::vector<std::string>&	getWhiteList();
		std::vector<chanel_mode>&	getActiveModes() ;
		client_map&					getclientMap();
		map_iterator				getClient(std::string name);

		void						setChanelName(std::string name);

		bool						setChanelModes(char mode);
		bool						unsetChanelModes(char mode);

		bool						setClientMode(map_iterator client, char mode);
		bool						unsetClientMode(map_iterator client, char mode);
		

		std::string 				listChanelModes();
		std::string 				listAllClientsModesAndNames(ListFunc listFunc);

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
		std::vector<chanel_mode>				_active_modes;
		client_map								_clients_in;
		std::string								_key;
};

#endif
