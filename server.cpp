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
		|| cmd == "PRIVMSG" || cmd == "QUIT" || cmd == "JOIN" || cmd == "PART"
		|| cmd == "KICK" || cmd == "INVITE")
	{
		pos = arg.find_first_not_of(" : \r\n");
		end_pos = arg.find_last_not_of(" \t\f\v\n\r");
		if (pos != string::npos && end_pos != string::npos)
			res = arg.substr(pos, end_pos - pos + 1);
		else if (pos != string::npos && end_pos == string::npos)
			res = arg.substr(pos, arg.size() - pos + 1);
		else
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

string	Server::check_nick_presence(string nick_toFind)//if client found return empty string
{
	for (size_t i = 0; i < this->clients.size(); i++)
		if (nick_toFind == this->clients[i]->nick)
			return "";
	return nick_toFind;
}
string	Server::check_channel_presence(string channel_toFind)//if channel found return empty string
{
	std::map<string, Channel *>::iterator it = channels.begin();
	for (; it != channels.end(); it++)
		if (channel_toFind == it->first)
			return "";
	return channel_toFind;
}

string	Server::check_nickNAMEs(std::vector<string> &cls) //client / channel vector
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

string	Server::check_channNAMEs(std::vector<string> &cls) //client / channel vector
{
	std::sort(cls.begin(), cls.end());
	std::vector<string>::iterator it = std::unique(cls.begin(), cls.end());
	if (it != cls.end())
		return (ERR_TOOMANYTARGETS(*it));
	string unfound;
	for (size_t i = 0; i < cls.size(); i++)
	{
		unfound = check_channel_presence(cls[i]);
		if (unfound != "")
			return (ERR_NOSUCHNICK(unfound));
	}
	return ("");
}

void	Server::leave_channels(Client * client, string channel)
{
	std::map<string, Channel*>::iterator it;
	size_t pos = channel.find(' '); // lime chat sending msg after chan_name
	if (pos != string::npos)
		channel.erase(pos);
	it = this->channels.find(channel);
	if (it == this->channels.end())
	{
		string dup = "403 * " + channel + " :No such channel";
		send_msg(client, dup);
	}
	else
	{
		std::map<string, Client*>::iterator _it = it->second->members.find(client->nick);
		if (_it == it->second->members.end())
		{
			string dup = "442 * " + channel + " :You're not on that channel";
			send_msg(client, dup);
		}
		else
		{
			for (std::map<string, Client*>::iterator i = it->second->members.begin(); i != it->second->members.end(); i++)
			{
				string msg = ":" + client->user_info() + " PART " + channel + "\r\n";
				send_msg(i->second, msg);
			}
			it->second->members.erase(client->nick);
			it->second->operators.erase(client->nick);
		}
		if (it->second->members.size() == 0) //erase channel when no member left in it
			this->channels.erase(it);
		else if (it->second->members.size() > 0 && it->second->operators.size() == 0)
			it->second->operators.insert(std::pair<string, Client *> (it->second->members.begin()->first, 
			it->second->members.begin()->second));
	}
}

int	Server::find_spaceInBetween(string str)
{
	for (size_t i = 1; i < str.size(); i++)
	{
		if (str[i] == ' ' && isalnum(str[i - 1]))
		{
			for (size_t j = i; j < str.size(); j++)
			{
				i = j;
				if (str[j] != ' ')
					break ;
			}
			if (isalnum(str[i]))
				return (i - 1);
		}
	}
	return 0;
}
