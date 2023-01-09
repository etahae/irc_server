#pragma once

#include "client.hpp"
#include <map>

# define ERR_CHANNELISFULL(channel)			("471 * " channel " :Cannot join channel (+l)")
# define ERR_BANNEDFROMCHAN(channel)		("474 * " channel " :Cannot join channel (+b)")
# define ERR_BADCHANNELKEY(channel)			("475 * " channel " :Cannot join channel (+k)")

namespace irc
{
	class Channel
	{
		public :
			string	ch_name;
			string	passwd;
			size_t	max_numbers;
			string	modes; //oip...
			// std::vector<Client *> operators;
			// std::vector<Client *> members;
			// std::vector<Client *> bans;

			std::map<string, Client *> members;
			std::map<string, Client *> bans;
			std::map<string, Client *> operators;

			Channel():ch_name(""),passwd(""),max_numbers(9999) {}
			Channel(string name, string pass):ch_name(name),passwd(pass),max_numbers(9999) {}
			
			void	joinChannel(string name, string pass, Client *cl)
			{
				string err = "";
				//check no pass or name given
				if(bans.find(cl->nick) != bans.end())	//ban member
					err = "474 * " + this->ch_name + " :Cannot join channel (+b)";
				else if (members.size() >= max_numbers)	//channel reach it's max member
					err = "471 * " + this->ch_name + " :Cannot join channel (+l)";
				else if (name == ch_name && pass != pass)	//password not match
					err = "475 * " + this->ch_name + " :Cannot join channel (+k)";
				else if (name == ch_name && pass == pass)
				{
					if(this->operators.size() == 0)
						operators.insert(std::pair<string,Client *> (cl->nick, cl));
					members.insert(std::pair<string,Client *> (cl->nick, cl));
				}
				if (err != "")
				{
					send_err(cl, err);
					return ;
				}
			}
			int	validateChannelName(const string &str)
			{
				string channelRegex = "ABCDEFGHIKLMNOPQRSTVXYZabcdefghijklmnopqrstuvwxyz\r\n";
				
				int position = str.find_last_not_of(channelRegex);
				cout << position << endl;
				if (position == 0)
					if (str[0] == '&' || str[0] == '#')
						return 1;
				return 0;
			}

			void	send_err(Client *client, string msg)
			{
				msg = msg + "\r\n";
				write(client->fd_socket, msg.c_str(), msg.size());
			}
	};
}


/*
4.2.3.1 Channel modes :
   Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>]

           o - give/take channel operator privileges;
           p - private channel flag;
           s - secret channel flag;
           i - invite-only channel flag;
           t - topic settable by channel operator only flag;
           n - no messages to channel from clients on the outside;
           m - moderated channel;
           l - set the user limit to channel;
1.3.1 channel operators:
			KICK    - Eject a client from the channel
			MODE    - Change the channel's mode
			INVITE  - Invite a client to an invite-only channel (mode +i)
			TOPIC   - Change the channel topic in a mode +t channel

4.2.1 Join message :
			JOIN <channel>{,<channel>} [<key>{,<key>}]
	...
*/
