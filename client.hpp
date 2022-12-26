#pragma once

#include "server.hpp"

class client
{
	public:
		//client authentication
		string	pass;
		string	nick;
		string	username;
		//file descriptor
		//vector channel

		//constructors
		client(){};
		client(char *Pass, char *Nick, char *Username) : pass(Pass), nick(Nick),username(Username)
		{}
};

