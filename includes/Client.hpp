/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 11:31:22 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/19 18:47:13 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

#include "Utils.hpp"

class Client
{
	public:
		Client(int socket, std::string nickname): _my_socket(socket), _nickname(nickname)
		{}

		Client(const Client& c):_my_socket(c._my_socket), _nickname(c._nickname)
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
				_nickname = c._nickname;
				_my_socket = c._my_socket;
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

	private:
		int							_my_socket;
		std::string 				_nickname;
		// We have to think about if it's the best way to use a map like this.
		// Indeed a Client can have many chanel with different role,
		std::map<int, std::string>	_my_chanel;
		Client(){}
};

#endif