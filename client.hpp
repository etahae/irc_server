#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_ntoa
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include "server.hpp"

using std::string;
using std::cout;
using std::endl;

namespace irc
{
	class Client
	{
		public:
			//client authentication
			string	pass;
			string	nick;
			string	username;
			string	ip_address;
			int		fd_socket;
			struct	sockaddr_in sock_addr;
			bool	verified;
			//vector channel

			//constructors
			Client():verified(false) {}
			Client(int Fd, struct	sockaddr_in Sock_addr, string ip_, string Pass = string(), string Nick = string(), string Username = string())
					: pass(Pass), nick(Nick),username(Username),  ip_address(ip_), fd_socket(Fd), sock_addr(Sock_addr)
			{
				verified = false;
			}

			string user_info()
			{
				return (this->nick + "!" + this->username + "@" + this->ip_address);
			}
	};
}
