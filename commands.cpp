#include "server.hpp"
#include "client.hpp"
#include <algorithm>

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
                string dup = "433 * " + client->nick + ":Nickname is already in use";
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
        int i = user.find(' ');
        client->username = user.substr(0, i);
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

void    Server::_NOTICE(string s_token, Client * client, string msg)
{
    if (s_token == "NOTICE\r\n" || s_token == "NOTICE\n" || (s_token == "NOTICE" && msg == "")) //hundle no notice command given // WE SHOULD HANDLE ONLY WHITE SPACES AFTER NOTICE
        return ;
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
                string letter = ":" + client->nick + " PRIVMSG " + string(nick_name) + " : " + msg_token;
                send_msg(receiver, letter);
            }
        }
    }
}

int	Server::_PRIVMSG(string s_token, Client * client, string msg)
{
	if (s_token == "PRIVMSG\r\n" || s_token == "PRIVMSG\n")
		return (send_msg(client, ERR_NORECIPIENT("PRIVMSG")), 1);
    else if (s_token == "PRIVMSG")
    {
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
        else
            sms = "";
        string valid_cls = check_nickNAMEs(cls);
		string valid_chan = check_channNAMEs(cls);
		if (valid_cls != "" && valid_chan != "")
			return (send_msg(client, valid_chan), 1);
        if (sms == "")
            return (send_msg(client, ERR_NOTEXTTOSEND), 1);
        size_t find_pts = sms.find(':');
        if (find_pts == string::npos)
            sms.insert(0, ": ");
        else if (sms[find_pts + 1] != ' ')
            sms.insert(find_pts + 1, " ");
        sms.insert(0, " ");
		if (valid_cls == "")
        	for (size_t i = 0; i < cls.size(); i++)
            	send_msg(this->find_client(cls[i]), ":" + client->nick + " PRIVMSG " + cls[i] + sms);
		else	//channels message
		{
			for (size_t i = 0; i < cls.size(); i++)
			{
				std::map<string, Channel *>::iterator target_chan = this->channels.find(cls[i]);
				if (target_chan->second->members.find(client->nick) == target_chan->second->members.end())
					return (send_msg(client, "442 * " + target_chan->first + " :You're not on that channel"), 0);
				for (std::map<string, Client *>::iterator it = target_chan->second->members.begin(); it != target_chan->second->members.end(); it++)
					if (it->first != client->nick)
            			send_msg(it->second, ":" + client->nick + " PRIVMSG " + cls[i] + sms);
			}
		}
	}
    return (0);
}

void    Server::_QUIT(string s_token, Client * client, int i, size_t index)
{
    if (s_token == "QUIT\r\n" || s_token == "QUIT\n" || s_token == "QUIT")
        this->disconnect(index, i, client->fd_socket);
}

void	Server::_JOIN(string s_token, Client * client, string chann)
{
	string chns;
	string keys;
	char*	str;
	std::vector<string> v_channels;
	std::vector<string> v_keys;
	if (s_token == "JOIN\r\n" || s_token == "JOIN\n" || (s_token == "JOIN" && chann == ""))
	{
		send_msg(client, ERR_NEEDMOREPARAMS("JOIN"));
		return ;
	}
	else if (s_token == "JOIN")
	{
		int i = find_spaceInBetween(chann);
		if (i)
		{
			chns = chann.substr(0, i);
			keys = chann.substr(i, chann.size() - i);
			str = strtok(const_cast<char *>(chns.c_str()), ",");//split channels with commas
			while (str != NULL)
			{
				string tmp = str;
				Server::trim_whiteSpaces(tmp);
				v_channels.push_back(tmp);
				str = strtok(NULL, ",");
			}
			str = strtok(const_cast<char *>(keys.c_str()), ",");//split keys with commas
			while (str != NULL)
			{
				string tmp = str;
				Server::trim_whiteSpaces(tmp);
				v_keys.push_back(tmp);
				str = strtok(NULL, ",");
			}
			if (v_keys.size() != v_channels.size())	//check that the same amount of channels and keys
			{
				send_msg(client, ERR_NEEDMOREPARAMS("JOIN_"));
				return ;
			}
			for (size_t i = 0; i < v_keys.size(); i++)
			{
				if (this->channels.find(v_channels[i]) == this->channels.end())
					this->channels.insert(std::pair<string, Channel *>(v_channels[i], new Channel(v_channels[i], v_keys[i])));
				this->channels.find(v_channels[i])->second->joinChannel(v_channels[i], v_keys[i], client);
			}
		}
		else
			send_msg(client, ERR_NEEDMOREPARAMS("_JOIN"));	//in case of JOIN failed
	}
}

void 	Server::_PART(string s_token, Client * client, string channs)
{
	if (s_token == "PART\r\n" || s_token == "PART\n" || (s_token == "PART" && channs == ""))
	{
		send_msg(client, ERR_NEEDMOREPARAMS("PART"));
		return ;
	}
	else if (s_token == "PART")
	{
		char *str = strtok(const_cast<char *>(channs.c_str()), ",");//split channels with commas
		string tmp = str;
		while (str != NULL)
		{
			tmp = str;
			size_t pos = tmp.find(':');
			if (pos != string::npos)
				tmp.erase(pos);
			Server::trim_whiteSpaces(tmp);
			this->leave_channels(client, tmp);
			str = strtok(NULL, ",");
		}
	}
}
