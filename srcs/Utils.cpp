/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alukongo <alukongo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/09 11:34:40 by idouidi           #+#    #+#             */
/*   Updated: 2023/04/28 19:24:22 by alukongo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/Control.hpp"

/*
* check if the port it's  1 < port < 65535
*/
bool check_port(std::string port) 
{

	int port_number = std::atoi(port.c_str());
	if (port.empty() || !port.find_first_not_of("0123456789") || port_number < 1025 || port_number > 65535)
	{
		std::cerr << RED << "error: " << RESET << "wrong format of the port:\n" \
		<< "the port must be a number which follows this rule ]1-65535[" << std::endl;
		return (false);
	}
	return (true);
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
		return (false);
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
		return (true);
	else
	{
		std::cerr << RED << "error: " << RESET << "wrong format of the password:\n" \
		<< " - At least  8 characters \n"  << " - At least  1 upper case\n" << " - At least  1 lower case\n"	\
		<< " - At least  1 digit" << std::endl;	
		return (false);
	}
}

std::string getDateTime() 
{
    time_t      currentTime;
    struct tm   *localTime;
    char        dateTime[80];

    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    strftime(dateTime, 80, "%Y-%m-%d %H:%M:%S", localTime);
    return (std::string(dateTime) + " GMT");
}

void printInServer(std::vector<std::string> cmd, Client& client)
{
    std::string msg;
    for (std::size_t i = 0; i < cmd.size(); i ++)
    {
        msg += (i +1 != cmd.size()) ? (cmd[i] + " ") : cmd[i];
    }
	std::cout << MAGENTA << "Message from the client[ " << CYAN << client.getMySocket() << MAGENTA << " ]: "\
	<< YELLOW "[" << CYAN << msg << YELLOW << "]" << RESET << std::endl;
}

std::vector<std::string> split(std::string str, const std::string charset)
{
    std::vector<std::string>    tokens;
    size_t                      pos = 0;

    while (pos != std::string::npos)
    {
        pos = str.find_first_of(charset);
        if (pos != std::string::npos)
        {
            std::string token = str.substr(0, pos);
            tokens.push_back(token);
            str = str.substr(pos + 1);
        }
    }
    for (std::size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] == "")
            tokens.erase(tokens.begin() + i);
    }
    return (tokens);
}

char    convertClientModeToChar(client_mode mode)
{
    switch (mode)
    {
        case SERVER_OPERATOR:
            return ('o');
        case CHANEL_OPERATOR:
            return ('O');
        case VOICE:
            return ('v');
        case HALF_OP:
            return ('h');
        case ADMIN:
            return ('a');
        case INVISIBLE:
            return ('i');
        case WALLOPS:
            return ('w');
        default:
            return (0);
    }
}

char    convertChanelModeToChar(chanel_mode mode)
{
    switch (mode)
    {
        case TOPIC_PROTECTION:
            return ('t');
        case NO_EXTERNAL_MSG:
            return ('n');
        case SECRET:
            return ('s');
        case MODERATED:
            return ('m');
        case INVITE_ONLY:
            return ('i');
        case PRIVATE:
            return ('p');
        case KEY:
            return ('k');
        case LIMIT:
            return ('l');
        default:
            return (0);
    }
}
