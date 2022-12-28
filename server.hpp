#pragma once

#define VERIFIED "\033[0;32m • \033[0m"

#include"client.hpp"

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
			if (s_token == "NICK") //set Nick_Name
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
