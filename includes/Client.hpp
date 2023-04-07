/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 11:31:22 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/06 18:48:41 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__
# include "Control.hpp"

class Client
{
	public:
		typedef std::map<Chanel, std::vector<chanel_mode> >	chanel_map;
		typedef chanel_map::iterator						map_iterator;
		
		Client(int socket, std::string token);
		Client(const Client& c);
		Client& 	operator=(const Client& c);
		~Client();

		
		int							getMySocket() const;
		char 						getMyUserMode() const;
		std::string 				getMyNickname() const;
		std::string 				getMyUserName() const;
		std::string					getToken() const;
		time_t 						getLastActiveTime() const;
		std::vector<client_mode>&	getActiveModes() ;
		chanel_map&					getChanelMap();
		
		void 						setStatusClient(bool status);
		void 						setNickName(std::string nick);
		void 						setUserName(std::string user);
		void 						setUserMode(char mode);
		void						setLastActiveTime();
		bool						setModes(char mode);
		bool						unsetModes(char mode);

		bool 						isNewClient() const;
		bool						isValidMode(char mode, client_mode& idx);
		bool						insertChanel(Chanel& chanel_to_add, chanel_mode chan_mode);

		template <typename T2>
		bool		operator==(T2& rhs) const { return (my_socket == rhs.my_socket); }

		template<typename T1>
		bool		operator<(T1& rhs) const{ return (token_ping < rhs.token_ping); }

	private:
	
		int													my_socket;
		std::string											token_ping;
		std::string 										nickname;
		std::string											username;
		std::vector<client_mode>							active_modes; //mode in server
		bool												new_client;
		time_t												last_active_time;
		chanel_map											my_chanels;
		
};

#endif