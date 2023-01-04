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
	if (cmd == "NICK" || cmd == "PASS" || cmd == "USER" || cmd == "NOTICE"
		|| cmd == "PRIVMSG")
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

void	Server::trim_whiteSpaces(string &str)
{
	size_t start = str.find_first_not_of(" \t");
	size_t	end = str.find_last_not_of(" \n\t\r");
	if (start != string::npos && end != string::npos)
		str = str.substr(start, end - start + 1);
	else if (start != string::npos)
		str = str.substr(start, str.size() - start + 1);
	else if (end != string::npos)
		str = str.substr(0, end - 1);
}

string	Server::check_nick_presence(string nick_toFind)
{
	for (size_t i = 0; i < this->clients.size(); i++)
		if (nick_toFind == this->clients[i]->nick)
			return "";
	return nick_toFind;
}

string	Server::check_nickNAMEs(std::vector<string> &cls)
{
	std::sort(cls.begin(), cls.end());
	std::vector<string>::iterator it = std::unique(cls.begin(), cls.end());
	if (it != cls.end())
		return (ERR_TOOMANYTARGETS(*it));
	string unfound;
	for (size_t i = 0; i < cls.size(); i++)
	{
		unfound = check_nick_presence(cls[i]);
		if (unfound != "")
			return (ERR_NOSUCHNICK(unfound));
	}
	return ("");
}