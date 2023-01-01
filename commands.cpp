#include "server.hpp"
#include "client.hpp"

using namespace irc;

void	Server::_NICK(string s_token, Client * client, string nick)
{
    int n = 0;
    if (s_token == "NICK\r\n" || s_token == "NICK\n" || nick == "") //hundle no nickname given // WE SHOULD HANDLE ONLY WHITE SPACES AFTER NICK
        write(client->fd_socket, ERR_NONICKNAMEGIVEN, strlen(ERR_NONICKNAMEGIVEN));
    else if (s_token == "NICK") //set Nick_Name
    {
        client->nick = nick;
        for (size_t i = 0; i < this->clients.size(); i++)
        {
            if (this->clients[i].nick == client->nick)
                n++;
            if (n > 1){
                string dup = "433 * " + client->nick + ":Nickname is already in use\r\n";
                write(client->fd_socket, dup.c_str(), strlen(dup.c_str()));
                client->nick = "";
                break ;
            }
        }
    }
}

void	Server::_USER(string s_token, Client * client, string user)
{
    if (s_token == "USER") //set User_Name
        client->username = user;
}

void	Server::_PASS(string s_token, Client * client, string pass)
{
    if (s_token == "PASS") //set Password
        client->pass = pass;
}

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
	if (cmd == "NICK" || cmd == "PASS" || cmd == "USER")
	{
		pos = arg.find_first_not_of(" :\r\n");
		end_pos = arg.find_last_not_of("\r\n");
		if (pos != string::npos && end_pos != string::npos)
			res = arg.substr(pos, end_pos + 1);
		else if (pos != string::npos && end_pos == string::npos)
			res = arg.substr(pos, arg.size());
		else if (pos == end_pos)
			res = "";
	}
}