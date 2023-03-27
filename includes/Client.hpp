/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 11:31:22 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/28 00:19:48 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

#include "Utils.hpp"

class Client
{
	public:

		Client(int socket): _my_socket(socket), new_client(1)
		{}

		Client(const Client& c):_my_socket(c._my_socket), _nickname(c._nickname), new_client(c.new_client)
		{
			if (!c._my_chanel.empty())
			{
				_my_chanel.clear();
				_my_chanel.insert(c._my_chanel.begin(), c._my_chanel.end());
			}
		}

		Client& operator=(const Client& c)
		{
			if (this != &c)
			{;
				_my_socket = c._my_socket;
				_nickname = c._nickname;
				new_client = c.new_client;
				if (!c._my_chanel.empty())
            	{
                	_my_chanel.clear();
                	_my_chanel.insert(c._my_chanel.begin(), c._my_chanel.end());
            	}
			}
			return (*this);
		}

		int	getMySocket() const { return (_my_socket); }
		std::string getMyNickname() const{ return (_nickname); }

		bool isNewClient() const  { return (new_client); }
		void setStatusClient(bool status) { new_client = status; }

		bool isNicknameUp() const { return (_nickname != ""); }
		void setNickName(std::string nick) { _nickname.assign(nick); }


	private:
		int							_my_socket;
		std::string 				_nickname;
		bool						new_client;
		// We have to think about if it's the best way to use a map like this.
		// Indeed a Client can have many chanel with different role,
		std::map<int, int>			_my_chanel;
		
		
};

#endif