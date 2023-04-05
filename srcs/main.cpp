/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:06:13 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/05 14:20:21 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

/*
* check if the port it's  1 < port < 65535
*/
bool check_port(std::string port) 
{

	int port_number = std::atoi(port.c_str());
	if (port.empty() || !port.find_first_not_of("0123456789") || port_number < 1 || port_number > 65535)
	{
		std::cerr << RED << "error: " << RESET << "wrong format of the port:\n" \
		<< "the port must be a number which follows this rule ]1-65535[" << std::endl;
		return false;
	}
	return true;
}

/*
* check if the password has:
* - At least 8 characters
* - At least 1 upper case
* - At least 1 lower case
* - At least 1 digit
*/
bool check_pswd(std::string pswd) 
{
	bool has_upper = false;
	bool has_lower = false;
	bool has_digit = false;
	
	if (pswd.length() < 8)
		return false;
	for(size_t i = 0; i < pswd.size(); i++)
	{
		if (isupper(pswd[i]))
			has_upper = true;
		else if (islower(pswd[i]))
			has_lower = true;
		else if (isdigit(pswd[i]))
			has_digit = true;
	}
	if (has_upper && has_lower && has_digit)
		return true;
	else
	{
		std::cerr << RED << "error: " << RESET << "wrong format of the password:\n" \
		<< " - At least  8 characters \n"  << " - At least  1 upper case\n" << " - At least  1 lower case\n"	\
		<< " - At least  1 digit" << std::endl;	
		return false;
	}
}

std::string getDateTime() 
{
    time_t currentTime;
    struct tm *localTime;
    char dateTime[80];

    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    strftime(dateTime, 80, "%Y-%m-%d %H:%M:%S", localTime);
    return (std::string(dateTime) + " GMT");
}

void printInServer(std::string msg, Client& client)
{
	std::cout << MAGENTA << "Message from the client[ " << CYAN << client.getMySocket() << MAGENTA << " ]: "\
	<< YELLOW "[" << CYAN << msg.substr(0, msg.size()- 2) << YELLOW << "]" << RESET << std::endl;
}

std::vector<std::string> split(std::string str, const std::string charset) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    while (pos != std::string::npos) {
        pos = str.find_first_of(charset);
        if (pos != std::string::npos) {
            std::string token = str.substr(0, pos);
            tokens.push_back(token);
            str = str.substr(pos + 1);
        }
        else {
            tokens.push_back(str);
        }
    }
	for (size_t i = 0; i < tokens.size(); i++)
		if (tokens[i] == "")
			tokens.erase(tokens.begin() + i);
    return tokens;
}

void start_server(char *port, char *pswd)
{
	int 	                                new_client;
	int                                     ret;
	int                                     fd_make_event;
	char	                                buf[BUFFER_SIZE] = {0};
	struct                                  sockaddr_in client_addr;
	Irc                                     irc(port, pswd);

	std::cout << CYAN <<"\n- _ - _ - _ - _ - WAITING FOR NEW CONNECTION\
 - _ - _ - _ - _ -" << RESET << std::endl;
	while (1)
	{
		fd_make_event = epoll_wait(irc.getEpollFd(), irc.getEventTab(),  MAX_EVENTS, -1);
		for (int i = 0; i < fd_make_event; i++)
		{
			// EVENT ON THE SERVER
			if (irc.getEvent(i).data.fd == irc.getServerFd())
			{
				socklen_t client_len = sizeof(client_addr);
				if ((new_client = accept(irc.getServerFd(), (struct sockaddr*)&client_addr, &client_len)) == -1)
				{
					perror("accept");
					continue;
				}
				else
					irc.addClient(new_client);
			}
			//EVENT ON A CLIENT
			else 
			{
				ret = read(irc.getEvent(i).data.fd, buf, BUFFER_SIZE);
				Client *current_client = irc.findClient(irc.getEvent(i).data.fd);

				if (ret == -1)
				{
					perror("read");
					exit(EXIT_FAILURE);
				}
				else if (ret == 0)
						irc.eraseClient(*current_client);
				else
				{
					printInServer(std::string(buf), *current_client);
					std::vector<std::string> cmd = split(std::string(buf), " \n\r");
					if (cmd[0] != "PONG" && current_client->isNewClient())
					{
						if ((irc.parsInfo(*current_client, cmd)) == 0)
						{
							irc.eraseClient(*current_client);
							continue ;
						}
					}
					else if (cmd[0] != "PONG" && current_client->isUserModeUp() != 0)
						irc.setClientMode(*current_client, cmd[0],  'i');
					else
						irc.execCmd(*current_client, cmd);
				}
				memset(buf, '\0', sizeof(buf));
			}
		}
	}
}


int	main(int ac , char *av[])
{
	if (ac != 3)
	{
		std::cout << RED << "error: " RESET << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1); 
	}
	else if (!check_port(av[1]) || !check_pswd(av[2]))
		return (1);
	// signal(SIGINT, sig_handler);
	try
	{
		start_server(av[1], av[2]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return (0);
}