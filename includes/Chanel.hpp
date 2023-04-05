/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asimon <asimon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 14:17:22 by asimon            #+#    #+#             */
/*   Updated: 2023/04/05 17:21:28 by asimon           ###   ########.fr       */
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

		bool		addClient(Client& client_to_add, client_mode chan_mode);
		bool		deleteClient(Client& client_to_delete);
		size_t		getNumClient() const;
		std::string list_client();

		template <typename T1>
		bool		operator<(T1& rhs) const { return (_id < rhs._id); }
		
		
	private:
		size_t									_id;
		std::string								_name;
		std::string								_topic;
		client_map								_clients_in;
};

#endif
