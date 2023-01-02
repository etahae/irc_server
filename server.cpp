#include "server.hpp"

using namespace irc;

void    Server::split(char *str, string &cmd, string &res)
{
	char* token;
	char* rest = NULL;
	rest = strdup(str);
	string arg = "";
	unsigned long pos = 0, end_pos = 0;

	token = strtok_r(rest, " ", &rest);
	cmd = token;    //NICK || USER || PASS ...
	arg = rest;     //the whole command after removing NICK, ...
	if (cmd == "NICK" || cmd == "PASS" || cmd == "USER" || cmd == "NOTICE")
	{
		pos = arg.find_first_not_of(" : \r\n");
		end_pos = arg.find_last_not_of(" \t\f\v\n\r");
		if (pos != string::npos && end_pos != string::npos)
			res = arg.substr(pos, end_pos - pos + 1);
		else if (pos != string::npos && end_pos == string::npos)
			res = arg.substr(pos, arg.size() - pos + 1);
		else		//(pos == end_pos)
			res = "";
	}
}

size_t  Server::params_calc(string params)
{
    char * strToken = const_cast<char *> (params.c_str());
    size_t count = 0;
    strToken = strtok ( strToken, " " );
    while ( strToken != NULL ) {
        strToken = strtok ( NULL, " " );
        count++;
    }
    return count;
}

Client * Server::find_client(string nick)
{
	for (size_t i = 0; i < this->clients.size(); i++)
		if (this->clients[i]->nick == nick)
			return this->clients[i];
	return (nullptr);
}