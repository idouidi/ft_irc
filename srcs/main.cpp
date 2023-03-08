/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouidi <idouidi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 15:06:13 by idouidi           #+#    #+#             */
/*   Updated: 2023/03/08 16:44:41 by idouidi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Utils.hpp"

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

void start_server(std::string port)
{
	// int 	                                new_client;
	// char	                                buf[BUFFER_SIZE] = {0};
    Irc                                     irc(port);

    irc.init_server();
	while (1)
// 	{
// 		std::cout << CYAN <<"\n- _ - _ - _ - _ - WAITING FOR NEW CONNECTION
//  - _ - _ - _ - _ -" << RESET << std::endl;
//         if ((new_client = accept(server_fd, (struct sockaddr *)&address, &address_len)) < 0) 
//         {
//             perror("accept");
//             exit(EXIT_FAILURE);
//         }
// 		read(new_client, buf, BUFFER_SIZE);
// 		std::cout << GREEN << "Message from the client: " << YELLOW << buf << RESET << std::endl;
//         memset(buf, '\0', sizeof(buf));
// 	    close(new_client);
// 	}
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
        start_server(std::string(av[1]));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }
    
	return (0);
}