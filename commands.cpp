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

void	Server::_NOTICE(string s_token, string msg)
{
    if (s_token == "NOTICE\r\n" || s_token == "NOTICE\n" || (s_token == "NOTICE" && msg == "")) //hundle no nickname given // WE SHOULD HANDLE ONLY WHITE SPACES AFTER NICK
        return ;
    else if (s_token == "NOTICE")
    {
        char * msg_token = strdup(const_cast<char *> (msg.c_str()));
        char * nick_name = strtok_r(msg_token, " ",
            &msg_token);
        cout <<"**"<< nick_name <<"**"<< endl;
        if (msg_token && nick_name)
        {
            Client * receiver = find_client(nick_name);
            if (receiver != nullptr)
            {
                string letter = ":server NOTICE " + string(nick_name) + ": " + msg_token;
                send_msg(receiver, letter);
            }
        }
    }
}