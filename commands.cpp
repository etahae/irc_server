#include "server.hpp"
#include "client.hpp"

using namespace irc;

void	Server::_NICK(string s_token, Client * client)
{
    int n = 0;
    if (s_token == "NICK\r\n" || s_token == "NICK\n") //hundle no nickname given // WE SHOULD HANDLE ONLY WHITE SPACES AFTER NICK
        write(client->fd_socket, ERR_NONICKNAMEGIVEN, strlen(ERR_NONICKNAMEGIVEN));
    else if (s_token == "NICK") //set Nick_Name
    {
        client->nick = std::strtok(NULL, " ");
        if (client->nick.find('\n') != string::npos)
            client->nick = client->nick.substr(0,client->nick.size() - 1);
        if (client->nick.find('\r') != string::npos)
            client->nick = client->nick.substr(0,client->nick.size() - 1);
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

void	Server::_USER(string s_token, Client * client)
{
    if (s_token == "USER") //set User_Name
    {
        client->username = std::strtok(NULL, " ");
        if (client->username.find('\n') != string::npos)
            client->username = client->username.substr(0,client->username.size() - 1);
        if (client->username.find('\r') != string::npos)
            client->username = client->username.substr(0,client->username.size() - 1);
    }
}

void	Server::_PASS(string s_token, Client * client)
{
    if (s_token == "PASS") //set Password
    {
        client->pass = std::strtok(NULL, " ");
        if (client->pass.find('\n') != string::npos)
            client->pass = client->pass.substr(0,client->pass.size() - 1);
        if (client->pass.find('\r') != string::npos)
            client->pass = client->pass.substr(0,client->pass.size() - 1);
    }
}