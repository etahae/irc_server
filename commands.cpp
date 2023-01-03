#include "server.hpp"
#include "client.hpp"

using namespace irc;

void	Server::_NICK(string s_token, Client * client, string nick)
{
    int n = 0;
    if (s_token == "NICK\r\n" || s_token == "NICK\n" || (s_token == "NICK" && nick == "")) //hundle no nickname given // WE SHOULD HANDLE ONLY WHITE SPACES AFTER NICK
        send_msg(client, ERR_NONICKNAMEGIVEN);
    else if (s_token == "NICK") //set Nick_Name
    {
        client->nick = nick;
        for (size_t i = 0; i < this->clients.size(); i++)
        {
            if (this->clients[i]->nick == client->nick)
                n++;
            if (n > 1){
                string dup = "433 * " + client->nick + ":Nickname is already in use\r\n";
                send_msg(client, dup);
                client->nick = "";
                break ;
            }
        }
    }
}

void	Server::_USER(string s_token, Client * client, string user)
{
    if (s_token == "USER\r\n" || s_token == "USER\n" || (s_token == "USER" && user == "")) //hundle no nickname given // WE SHOULD HANDLE ONLY WHITE SPACES AFTER NICK
        send_msg(client, ERR_NEEDMOREPARAMS("USER"));
    else if (s_token == "USER") //set User_Name
    {
        if (client->username != "")
        {
            send_msg(client, ERR_ALREADYREGISTRED);
            return ;
        }
        if (this->params_calc(user) < 4)
        {
            send_msg(client, ERR_NEEDMOREPARAMS("USER"));
            return ;
        }
        client->username = user;
    }
}

void	Server::_PASS(string s_token, Client * client, string pass)
{
    if (s_token == "PASS\r\n" || s_token == "PASS\n" || (s_token == "PASS" && pass == "")) //hundle no nickname given // WE SHOULD HANDLE ONLY WHITE SPACES AFTER NICK
        send_msg(client, ERR_NEEDMOREPARAMS("PASS"));
    else if (s_token == "PASS") //set Password
    {
        if (client->pass != "")
        {
            send_msg(client, ERR_ALREADYREGISTRED);
            return ;
        }
        if (pass != this->password)
        {
            send_msg(client, ERR_PASSWDMISMATCH);
            return ;
        }
		client->pass = pass;
    }
}

Client	*Server::_NOTICE(string s_token, string msg)
{
    if (s_token == "NOTICE\r\n" || s_token == "NOTICE\n" || (s_token == "NOTICE" && msg == "")) //hundle no notice command given // WE SHOULD HANDLE ONLY WHITE SPACES AFTER NOTICE
        return nullptr;
    else if (s_token == "NOTICE")
    {
        char * msg_token = strdup(const_cast<char *> (msg.c_str()));
        char * nick_name = strtok_r(msg_token, " ",
            &msg_token);
        if (msg_token && nick_name)
        {
            Client * receiver = find_client(nick_name);
            if (receiver != nullptr)
            {
                string letter = ":server NOTICE " + string(nick_name) + ": " + msg_token;
                send_msg(receiver, letter);
				return receiver;
            }
        }
    }
	return nullptr;
}

void	trim_whiteSpaces(string &str)
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

int	Server::check_nickNAMEs(std::vector<string> &vec)
{
	size_t count = 0;
	for (size_t ii = 0; ii < this->clients.size(); ii++)
	{
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (vec[i] == clients[ii]->nick)
			{
				count++;
				if (count > 1)
					return (1);
			}
		}
		if (count == 0)
			return (2);
		count = 0;
	}
	if (vec.size() > clients.size())
		return 1;
	return (0);
}

int	Server::_PRIVMSG(string s_token, Client * client, string msg)
{
	if (s_token == "PRIVMSG\r\n" || s_token == "PRIVMSG\n")
		return (send_msg(client, ERR_NORECIPIENT("PRIVMSG")), 0);
	// if (s_token == "PRIVMSG" && msg == "")
	// 	return (ERR_NOTEXTTOSEND, 0);
	// size_t  i = 0;
	std::vector<string> cls;

	char *client_nick;
	string	result;
	string	sms;

	client_nick = strtok(const_cast<char *> (msg.c_str()), ",");
	while (client_nick != NULL)
	{
		result = client_nick;
		trim_whiteSpaces(result);
		cls.push_back(result);
		client_nick = strtok(NULL, ",");
	}
	sms = *(cls.end() - 1);
	size_t start = sms.find_first_of(" ");
	if (start != string::npos)
	{
		*(cls.end() - 1) = sms.substr(0, start);
		sms = sms.substr(start + 1, sms.length() - 1);
	}
	if (!this->check_nickNAMEs(cls))
		cout << "VALID\n";
	else
		cout << "NOOOOO\n";
	return 0;
}

//To_DO :
//MULTI client in PRIVMSG arguments
//CHECK repetitive clients if found return error :
//		ERR_TOOMANYTARGETS
//                    "<target> :Duplicate recipients. No message 