/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 11:31:22 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/06 14:14:13 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__
# include "Control.hpp"

class Client
{
	public:

		enum									chanel_mode { I, W, O, P, S, T, N, M, L, B, K, V };
		typedef std::map<Chanel, chanel_mode>	chanel_map;
		typedef chanel_map::iterator			map_iterator;
		
		Client(int socket, std::string token);
		Client(const Client& c);
		Client& 	operator=(const Client& c);
		~Client();

		
		int			getMySocket() const;
		std::string getMyNickname() const;
		std::string getMyUserName() const;
		char 		getMyUserMode() const;
		time_t 		getLastActiveTime() const;
		chanel_map&	getChanelMap();
		std::string	getToken() const;
		
		void 		setStatusClient(bool status);
		void 		setNickName(std::string nick);
		void 		setUserName(std::string user);
		void 		setUserMode(char mode);
		void		setLastActiveTime();

		bool 		isNewClient() const;
		bool 		isUserModeUp() const;

		bool		insertChanel(Chanel& chanel_to_add, chanel_mode chan_mode);


		template <typename t2>
		bool		operator==(t2& rhs) const { return (my_socket == rhs.my_socket); }

		template<typename T1>
		bool		operator<(T1& rhs) const{ return (token_ping < rhs.token_ping); }

	private:
	
		int										my_socket;
		std::string								token_ping;
		std::string 							nickname;
		std::string								username;
		bool									new_client;
		char									user_mode;
		time_t									last_active_time;
		std::map<Chanel, chanel_mode>			my_chanels;
		
};

#endif