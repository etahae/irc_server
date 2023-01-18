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

			std::map<string, bool>	bans;
			std::map<string, Client *>	members;
			std::map<string, Client *>	operators;
			std::map<string, Client *>	moderators;
			std::map<string, int> 		invited_clients;	 

			Channel():ch_name(""),passwd(""),max_numbers(9999) {}
			Channel(string name, string pass):ch_name(name),passwd(pass),max_numbers(9999) {}
			
			string names_list()
			{
				string list = "";
				for (std::map<string, Client *>::iterator it = members.begin(); it != members.end(); it++)
					if (operators.find(it->first) == operators.end())
						list += it->first + " ";
				for (std::map<string, Client *>::iterator it = operators.begin(); it != operators.end(); it++)
					list += "@" + it->first + " ";
				list.erase(list.begin() + list.size() - 1);
				return list;
			}

			void	joinChannel(string name, const string &pass, Client *cl)
			{
				string err = "";
				//check no pass or name given
				if (!validateChannelName(name))
					return ;
				if(bans.find(cl->user_info()) != bans.end() || bans.find("*!*@*") != bans.end())	//ban member (ex : MODE &ch_name +b user_name!realname@ip_address) (realname can be placed with *)
					err = "474 * " + this->ch_name + " :Cannot join channel (+b)";
				else if (members.size() >= max_numbers)	//channel reach it's max member
					err = "471 * " + this->ch_name + " :Cannot join channel (+l)";
				else if (pass != passwd)	//password not match
					err = "475 * " + this->ch_name + " :Cannot join channel (+k)";
				else if (this->members.find(cl->nick) != this->members.end())
					err = "443 * " + cl->nick + " " + ch_name + " :is already on channel";
				else if (this->modes.find('i') != string::npos)
				{
					if (this->invited_clients.find(cl->nick) == this->invited_clients.end())
						err = "473 * " + this->ch_name + " :Cannot join channel (+i)";
					else
						insert_in_members(cl);
				}
				else if (this->operators.size() == 0)
					insert_in_operator(cl);
				else
					insert_in_members(cl);
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
				if (position == 0)
					if (str[0] == '&' || str[0] == '#')
						return 1;
				return 0;
			}
			void	insert_in_operator(Client *cl)
			{
				this->operators.insert(std::pair<string,Client *> (cl->nick, cl));
				this->members.insert(std::pair<string,Client *> (cl->nick, cl));
				//send message to irc clients to verify that they joined the channel
				this->modes = "nt";
				string msg = ":" + cl->user_info() + " JOIN " + this->ch_name + "\r\n"
				":Rαɠɳαɾöƙ MODE " + this->modes + " +nt\r\n"
				":Rαɠɳαɾöƙ 353 " + cl->nick + " = " + this->ch_name + " :@" + cl->nick + "\r\n"
				":Rαɠɳαɾöƙ 366 " + cl->nick + " " + this->ch_name + " :End of /NAMES list.";
				send_err(cl, msg);
			}
			void	insert_in_members(Client *cl)
			{
				members.insert(std::pair<string,Client *> (cl->nick, cl));
				string msg = ":" + cl->user_info() + " JOIN " + this->ch_name + "\r\n"
				":Rαɠɳαɾöƙ 332 " + cl->nick + " " + this->ch_name + " :This is my cool channel!  https://irc.com\r\n"
				":Rαɠɳαɾöƙ 333 " + cl->nick + " " + this->ch_name + " " + operators.begin()->second->user_info() + " 1547691506\r\n"
				":Rαɠɳαɾöƙ 353 " + cl->nick + " @ " + this->ch_name + " :" + names_list() + "\r\n"
				":Rαɠɳαɾöƙ 366 " + cl->nick + " " + this->ch_name + " :End of /NAMES list.";
				send_err(cl, msg);
				for (std::map<string, Client*>::iterator it = members.begin(); it != members.end(); it++)
				{
					msg = ":" + cl->user_info() + " JOIN " + this->ch_name + "\r\n";
					send_err(it->second, msg);
				}
			}
			void	send_err(Client *client, string msg)
			{
				msg = msg + "\r\n";
				send(client->fd_socket, msg.c_str(), msg.size(), 0);
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
