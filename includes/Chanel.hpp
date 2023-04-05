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
		enum mode { I, S, W, O, P, T, N, M, L, B, K, V };
		
		Chanel(std::string, size_t);
		~Chanel();
		bool		deleteClient(Client& client_to_delete);
		bool		addClient(Client& client_to_add, mode mode);
		size_t		getNumClient() const;

		template <typename T1>
		bool		operator<(T1& rhs) const {
			return (_id < rhs._id);
		};
		
		
	private:
		size_t								_id;
		std::string							_name;
		std::string							_topic;
		std::map<Client, mode>				_clients_in;
};

#endif
