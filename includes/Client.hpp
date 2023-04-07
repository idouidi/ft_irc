/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asimon <asimon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 11:31:22 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/07 14:19:30 by asimon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__
# include "Control.hpp"

class Client
{
	public:
		typedef std::map<Chanel, std::vector<chanel_mode_e> >	chanel_map;
		typedef chanel_map::iterator						map_iterator;
		
		Client(int socket, std::string token);
		Client(const Client& c);
		Client& 	operator=(const Client& c);
		~Client();

		
		int							getMySocket() const;
		void 						getMyUserMode() const;
		std::string 				getMyNickname() const;
		std::string 				getMyUserName() const;
		std::string					getToken() const;
		time_t 						getLastActiveTime() const;
		std::vector<client_mode_e>&	getActiveModes() ;
		chanel_map&					getChanelMap();
		Chanel&						getCurentChanel() const;
		
		void 						setStatusClient(bool status);
		void 						setNickName(std::string nick);
		void 						setUserName(std::string user);
		void						setLastActiveTime();
		void						setCurrentChanel(Chanel& Chanel);

		bool						setModes(char mode);
		bool						unsetModes(char mode);

		bool 						isNewClient() const;
		bool						isValidMode(char mode, client_mode_e& idx);
		bool						insertChanel(Chanel& chanel_to_add, std::vector<chanel_mode_e> chan_mode);

		template <typename T2>
		bool		operator==(T2& rhs) const { return (my_socket == rhs.my_socket); }

		template<typename T1>
		bool		operator<(T1& rhs) const{ return (token_ping < rhs.token_ping); }

	private:
	
		int													my_socket;
		std::string											token_ping;
		std::string 										nickname;
		std::string											username;
		std::vector<client_mode_e>							active_modes; //mode in server
		bool												new_client;
		time_t												last_active_time;
		chanel_map											my_chanels;
		Chanel*												current_chanel;
		
};

#endif