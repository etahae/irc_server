#include "server.hpp"

using namespace irc;

void	Server::_o(char sign, string _channel, string _nick, Client * _client)
{
    std::map<string, Channel *>::iterator chan = this->channels.find(_channel);
    if (chan != this->channels.end())
    {
        std::map<string, Client *>::iterator oper = chan->second->operators.find(_client->nick);
        if (oper != chan->second->operators.end())
        {
            if (sign == '+')
                chan->second->operators.insert(std::pair<string, Client *> (_nick, find_client(_nick)));
            else if (sign == '-')
                chan->second->operators.erase(_nick);
        }
        else
            send_msg(_client, ERR_CHANOPRIVSNEEDED(_channel));
    }
    else
        send_msg(_client, ERR_NOSUCHCHAN(_channel));
}

void	Server::_i(char sign)
{
    if (sign == '+')
        return;
    else if (sign == '-')
        return;
}

void	Server::_p(char sign)
{
    if (sign == '+')
        return;
    else if (sign == '-')
        return;
}

void	Server::_t(char sign)
{
    if (sign == '+')
        return;
    else if (sign == '-')
        return;
}

void	Server::_l(char sign)
{
    if (sign == '+')
        return;
    else if (sign == '-')
        return;
}

void	Server::_b(char sign)
{
    if (sign == '+')
        return;
    else if (sign == '-')
        return;
}

void	Server::_m(char sign)
{
    if (sign == '+')
        return;
    else if (sign == '-')
        return;
}

void	Server::_v(char sign)
{
    if (sign == '+')
        return;
    else if (sign == '-')
        return;
}

void	Server::_k(char sign)
{
    if (sign == '+')
        return;
    else if (sign == '-')
        return;
}

