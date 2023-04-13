/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:04:28 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/13 19:07:54 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IRC_HPP__
#define __IRC_HPP__

# include "Control.hpp"

class Irc
{
	public:
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		/*	
		*	📌 CONSTRUCTOR / DESTRCUTOR
		*/

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

	Irc(char *port, char *pswd);

	~Irc();

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		/*	
		*	📌 GETTER
		*/

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

	int getServerFd() const;
		
	int getEpollFd() const;
		
	struct epoll_event getEvent(int i) const;
		
	struct epoll_event* getEventTab();
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

		/*	
		*	📌 MEMBER FUNCTION
		*/

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
	
	void init_server();
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
	
	void addClient(int client_fd);
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
	
	void eraseClient(Client* client);
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

	bool parsInfo(Client* client, std::vector<std::string> cmd);
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
	
	void sendMessagetoClient(Client* client, std::string msg);
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

	bool setClientMode(Client* client, std::string cmd, char mode);
	bool unsetClientMode(Client* client, std::string cmd, char mode);

	// bool setOtherClientMode(Client* client, Client* other, std::string cmd, char mode);
	// bool unsetOtherClientMode(Client* client, Client* other, std::string cmd, char mode);

	bool setChanelMode(Client* client, Chanel* chanel, std::string cmd, char mode);
	bool unsetChanelMode(Client* client, Chanel* chanel, std::string cmd, char mode);

	bool setOtherClientChanelMode(Client* client, Client* other, Chanel* chanel, std::string cmd, char mode);
	bool unsetOtherClientChanelMode(Client* client, Client* other, Chanel* chanel, std::string cmd, char mode);
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/


	Client* findClient(int fd_client);
	Client* findClient(std::string nick);

	Chanel* findChanel(std::string chanel_name);
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

	bool ping(Client*, std::vector<std::string>);
	bool pong(Client*, std::vector<std::string>);
	bool mode(Client*, std::vector<std::string>);
	bool who(Client*, std::vector<std::string>);
	bool whois(Client*, std::vector<std::string>);
	bool whowas(Client*, std::vector<std::string>);
	bool join(Client*, std::vector<std::string>);
	bool part(Client*, std::vector<std::string>);
	bool privatemsg(Client*, std::vector<std::string>);
	bool list(Client*, std::vector<std::string>);
	bool nick(Client*, std::vector<std::string>);
	bool invite(Client*, std::vector<std::string>);
	bool kick(Client*, std::vector<std::string>);
	bool quit(Client*, std::vector<std::string>);

	bool execCmd(Client* client , std::vector<std::string> cmd);
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

	void closeServer();
/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/

	void CheckChanelInfo(Chanel* chanel);
	void CheckClientInfo(std::map<Chanel, client_mode>& chanel);

/*	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	:	*/
	
	private:
	int										server_fd;
	int										epoll_fd;
	struct epoll_event						event;
	struct epoll_event						events[MAX_EVENTS];
	struct sockaddr_in 						server_addr;
	std::vector<Chanel*> 					_chanel;
	std::vector<Client*>					_client;
	std::string 							server_pswd;

	Irc() {}

	bool goodPassword(Client* client, std::string pswd);
	bool isNicknameAvaible(std::string nickname);
	std::string clientLastActiveTime(Client* client);
		
};


#endif

