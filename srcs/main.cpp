/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:06:13 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/28 16:14:32 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Control.hpp"

bool run = 1;

void signalhandler(int sign)
{
	if (sign == SIGINT || sign == SIGQUIT)
		run = 0;
}

void start_server(char *port, char *pswd)
{
	int 	                                new_client;
	int                                     ret;
	int                                     fd_make_event;
	char	                                buf[BUFFER_SIZE] = {0};
	struct                                  sockaddr_in client_addr;
	Irc                                     irc(port, pswd);

	signal(SIGINT, signalhandler);
	signal(SIGQUIT, signalhandler);
	std::cout << CYAN <<"\n- _ - _ - _ - _ - WAITING FOR NEW CONNECTION\
 - _ - _ - _ - _ -" << RESET << std::endl;
	while (run)
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
					irc.eraseClient(current_client);
				else if (std::string(buf).find('\n') == std::string::npos)
					current_client->setCmdLine(current_client->getCmdLine() + buf);
				else
				{
					std::vector<std::string> cmd = split(current_client->getCmdLine() + std::string(buf), " \t\n\r");
					current_client->getCmdLine().clear();
					if (cmd.size() > 0)
					{
						printInServer(cmd, *current_client);
						if (current_client->isNewClient())
						{
							if ((irc.parsInfo(current_client, cmd)) == 0)
							{
								irc.eraseClient(current_client);
								continue ;
							}
						}
						else
							irc.execCmd(current_client, cmd);
					}
				}
				memset(buf, '\0', sizeof(buf));
			}
		}
	}
		std::cout << "ici" << std::endl;
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
