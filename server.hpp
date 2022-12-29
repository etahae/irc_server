#pragma once

#define VERIFIED "\033[0;32m • \033[0m"
#define DISCONNECTED "\033[0;31m • \033[0m"

#include"client.hpp"

// REPLIES
#define RPL_LIST(channel, usersCount)			("322 * #" + channel + " " + usersCount)
#define RPL_LISTEND								"323 * :End of LIST"



// ERRORS

# define ERR_NONICKNAMEGIVEN					"431 * ERR_NONICKNAMEGIVEN:No nickname given"
# define ERR_NEEDMOREPARAMS(command)			("461 * " + command + ": Not enough parameters")
# define ERR_ALREADYREGISTRED					"462 * :Unauthorized command (already registered)"
# define ERR_PASSWDMISMATCH						"464 * :Password incorrect"
# define ERR_NICKNAMEINUSE(nick)				("433 * " + nick + ":Nickname is already in use")
# define ERR_RESTRICTED							"484 * :Your connection is restricted!"
# define ERR_NOTONCHANNEL(channel)				("442 * " + channel + ":You're not on that channel")
# define ERR_NOSUCHCHANNEL(channel)				("403 * #" + channel + ":No such channel")
# define ERR_CHANOPRIVSNEEDED(channel)			("482 * #" + channel + ":You're not channel operator")
# define ERR_USERNOTINCHANNEL(nick, channel)	("441 * " + nick + " #" + channel + ":They aren't on that channel")
# define ERR_BADCHANMASK(channel)				("476 * #" + channel + ":Bad Channel Mask")

class Server{
	public :

		string				password;
		int					port_number;
		int					_socket;
		std::vector<Client>	clients;
		struct sockaddr_in	server_addr;

		~Server()
		{
			clients.clear();
		}

		int fatal_error(std::string msg)
		{
			std::cerr << "error: " << msg << std::endl;
			return EXIT_FAILURE;
		}

		int init_server(char **argv)
		{
			this->password = argv[2];
			this->port_number = std::atoi(argv[1]);
			this->_socket = socket(AF_INET, SOCK_STREAM, 0);
			if (this->_socket < 0)
				return (this->fatal_error("socket failure"));
			bzero((char *) &this->server_addr, sizeof(this->server_addr));
			this->server_addr.sin_family = AF_INET;
			this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			this->server_addr.sin_port = htons(this->port_number);
			if (bind(this->_socket, (struct sockaddr *) &this->server_addr,
					sizeof(this->server_addr)) < 0)
				return (this->fatal_error("bind failure"));
			if (listen(this->_socket, 5) < 0)
				return (this->fatal_error("listen failure"));
			return 0;
		}

		int	client_verifying(char * cmd, Client * client)
		{
			if (!cmd || *cmd == 0)
				return 0;
			if (client->verified)
				return 1;
			char *token;
			string s_token;
			token = std::strtok(cmd, " ");
			s_token = token;
			cout << "++" << endl;
			if (s_token == "NICK\r\n" || s_token == "NICK\n") //hundle no nickname given
				write(client->fd_socket, ERR_NONICKNAMEGIVEN, strlen(ERR_NONICKNAMEGIVEN));
			else if (s_token == "NICK") //set Nick_Name
			{
				client->nick = std::strtok(NULL, " ");
				if (client->nick.find('\n') != string::npos)
					client->nick = client->nick.substr(0,client->nick.size() - 1);
				if (client->nick.find('\r') != string::npos)
					client->nick = client->nick.substr(0,client->nick.size() - 1);
			}
			if (s_token == "PASS") //set Password
			{
				client->pass = std::strtok(NULL, " ");
				if (client->pass.find('\n') != string::npos)
					client->pass = client->pass.substr(0,client->pass.size() - 1);
				if (client->pass.find('\r') != string::npos)
					client->pass = client->pass.substr(0,client->pass.size() - 1);
			}
			if (s_token == "USER") //set User_Name
			{
				client->username = std::strtok(NULL, " ");
				if (client->username.find('\n') != string::npos)
					client->username = client->username.substr(0,client->username.size() - 1);
				if (client->username.find('\r') != string::npos)
					client->username = client->username.substr(0,client->username.size() - 1);
			}
			if (client->nick != "" && client->username != "" && client->pass == this->password)
				return 1;
			return 0;
		}
};
