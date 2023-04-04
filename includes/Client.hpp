/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asimon <asimon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 11:31:22 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/04 16:23:51 by asimon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

#include "Control.hpp"

class Client
{
	public:

		Client(int socket, std::string token): my_socket(socket), token_ping(token), new_client(1), user_mode(0), last_active_time(time(0))
		{}

		Client(const Client& c):my_socket(c.my_socket), token_ping(c.token_ping), nickname(c.nickname), new_client(c.new_client), 
		user_mode(c.user_mode), last_active_time(c.last_active_time)
		{
			if (!c.my_chanel.empty())
			{
				my_chanel.clear();
				my_chanel.insert(c.my_chanel.begin(), c.my_chanel.end());
			}
		}

		Client& operator=(const Client& c)
		{
			if (this != &c)
			{;
				my_socket = c.my_socket;
				token_ping = c.token_ping;
				nickname = c.nickname;
				new_client = c.new_client;
				user_mode = c.user_mode;
				last_active_time =  c.last_active_time;
				if (!c.my_chanel.empty())
            	{
                	my_chanel.clear();
                	my_chanel.insert(c.my_chanel.begin(), c.my_chanel.end());
            	}
			}
			return (*this);
		}

		int			getMySocket() const { return (my_socket); }
		std::string getMyNickname() const { return (nickname); }
		std::string getMyUserName() const { return (username); }
		char 		getMyUserMode() const { return (user_mode); }
		time_t 		getLastActiveTime() const { return (last_active_time); }
		std::string	getToken() const { return (token_ping);}
		
		void 		setStatusClient(bool status) { new_client = status; }
		void 		setNickName(std::string nick) { nickname.assign(nick); }
		void 		setUserName(std::string user) { username.assign(user); }
		void 		setUserMode(char mode) { user_mode = mode; }
		void		setLastActiveTime() {last_active_time = time(0); }

		bool 		isNewClient() const  { return (new_client); }
		bool 		isUserModeUp() const { return (user_mode == 0);  }

	private:
		int							my_socket;
		std::string				token_ping;
		std::string 				nickname;
		std::string					username;
		bool						new_client;
		char						user_mode;
		time_t						last_active_time;
		// We have to think about if it's the best way to use a map like this.
		// Indeed a Client can have many chanel with different role,
		std::map<int, int>			my_chanel;
		
		
};

#endif