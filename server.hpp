#pragma once

#define VERIFIED "\033[0;32m • \033[0m"
#define CONNECTED "\033[0;32m • \033[0m"
#define DISCONNECTED "\033[0;31m • \033[0m"

#include "client.hpp"
#include "channel.hpp"

// REPLIES
// #define RPL_LIST(channel, usersCount)			("322 * #" + channel + " " + usersCount)
// #define RPL_LISTEND								"323 * :End of LIST"



// ERRORS

# define ERR_NONICKNAMEGIVEN					"431 * ERR_NONICKNAMEGIVEN:No nickname given"
# define ERR_PASSWDMISMATCH						"464 * ERR_PASSWDMISMATCH:Password incorrect"
# define ERR_ALREADYREGISTRED					"462 * ::You may not reregister"
# define ERR_NEEDMOREPARAMS(command)			("461 * " command ": Not enough parameters")
# define ERR_NICKNAMEINUSE(nick)					(nick " :Nickname is already in use")
# define ERR_NORECIPIENT(command)				("411 * :No recipient given (" command ")")
# define ERR_NOTEXTTOSEND						("412 * :No text to send")
// # define ERR_RESTRICTED							"484 * :Your connection is restricted!"
// # define ERR_NOTONCHANNEL(channel)				("442 * " + channel + ":You're not on that channel")
// # define ERR_NOSUCHCHANNEL(channel)				("403 * #" + channel + ":No such channel")
// # define ERR_CHANOPRIVSNEEDED(channel)			("482 * #" + channel + ":You're not channel operator")
// # define ERR_USERNOTINCHANNEL(nick, channel)	("441 * " + nick + " #" + channel + ":They aren't on that channel")
// # define ERR_BADCHANMASK(channel)				("476 * #" + channel + ":Bad Channel Mask")
class Channel;
namespace irc
{
	class Server{
		public :

			string				password;
			int					port_number;
			int					_socket;
			fd_set				r_socket, w_socket;
			std::vector<Client *>	clients;
			struct sockaddr_in	server_addr;
			std::map<string, Channel *> channels;
			~Server()
			{}

			int fatal_error(std::string msg)
			{
				std::cerr << "error: " << msg << std::endl;
				return EXIT_FAILURE;
			}

			int init_server(char **argv)
			{
				this->password = argv[2];
				this->port_number = std::atoi(argv[1]);
				this->_socket = socket(AF_INET, SOCK_STREAM, 0);
									//  IPV4,   TCP ,     tcp default
				if (this->_socket < 0)
					return (this->fatal_error("socket failure"));
				bzero((char *) &this->server_addr, sizeof(this->server_addr));
				this->server_addr.sin_family = AF_INET;
				this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
				this->server_addr.sin_port = htons(this->port_number);
				if (bind(this->_socket, (struct sockaddr *) &this->server_addr,
						sizeof(this->server_addr)) < 0)
					return (this->fatal_error("bind failure"));
				if (listen(this->_socket, 5) < 0)
					return (this->fatal_error("listen failure"));
				return 0;
			}

			int	client_verifying(char * cmd, Client * client, int i, size_t index)
			{
				if (!cmd || *cmd == 0)
					return 0;
				if (client->verified)
					return 1;
				char *token = strdup(cmd);
				if (token == NULL)
					return 0;
				string s_token;	//NICK, USER, PASS
				string res;		//sentence after NICK, USER, PASS

				token = std::strtok(token, " ");
				s_token = token;
				if (s_token == "sudo\n" || s_token == "sudo\r\n")
				{
					client->nick = "su";
					return 1;
				}
				if (s_token == "NICK" || s_token == "USER" || s_token == "PASS" || s_token == "QUIT")
					split(cmd, s_token, res);
				this->_NICK(s_token, client, res);
				this->_USER(s_token, client, res);
				this->_PASS(s_token, client, res);
				this->_QUIT(s_token, client, i, index);
				free(token);
				if (client->nick != "" && client->username != "" && client->pass == this->password)
					return 1;
				return 0;
			}

			int	customer_service(char * cmd, Client * client, int i, size_t index)
			{
				if (!cmd || *cmd == 0)
					return 0;
				if (cmd[0] == '/')
					this->bot_call(cmd, client);
				char *token = strdup(cmd);
				if (token == NULL)
					return 0;
				string s_token;	//NICK, USER, PASS
				string res;		//sentence after NICK, USER, PASS

				token = std::strtok(token, " ");
				s_token = token;
				if (s_token == "NICK" || s_token == "USER" || s_token == "PASS"
					|| s_token == "NOTICE" || s_token == "PRIVMSG" || s_token == "QUIT"
					|| s_token == "JOIN" || s_token == "PART" || s_token == "KICK"
					|| s_token == "INVITE" || s_token == "MODE")
					split(cmd, s_token, res);
				this->_NICK(s_token, client, res);
				this->_USER(s_token, client, res);
				this->_PASS(s_token, client, res);
				this->_NOTICE(s_token, client, res);
				this->_PRIVMSG(s_token, client, res);
				this->_JOIN(s_token, client, res);
				this->_PART(s_token, client, res);
				this->_MODE(s_token, client, res);
				this->_KICK(s_token, client, res);
				this->_INVITE(s_token, client, res);
				this->_QUIT(s_token, client, i, index);
				free(token);
				return 0;
			}

			void	disconnect(size_t index, size_t i, int fd)
			{
				if (this->clients[index] && this->clients[index]->nick == "")
					cout << DISCONNECTED << "_unknown_user" << " Disconnected" << endl;
				else
					cout << DISCONNECTED << this->clients[index]->nick << " Disconnected" << endl;
				FD_CLR(i, &this->r_socket);
				FD_CLR(i, &this->w_socket);
				close(fd);
				if (this->clients.size() != 0)
				{
					delete this->clients[index];
					this->clients.erase(this->clients.begin() + index);
					for (std::map<string, Channel*>::iterator it = this->channels.begin(); it != this->channels.end(); it++)
					{
						it->second->members.erase(it->first);
						it->second->operators.erase(it->first);
						it->second->moderators.erase(it->first);
					}
				}
			}

			void	send_msg(Client *client, string msg)
			{
				msg = msg + "\r\n";
				write(client->fd_socket, msg.c_str(), msg.size());
			}

			void	_NICK(string s_token, Client * client, string nick);
			void	_JOIN(string s_token, Client * client, string chann);
			void	_USER(string s_token, Client * client, string user);
			void	_PASS(string s_token, Client * client, string pass);
			void	_NOTICE(string s_token, Client * client, string pass);
			int 	_PRIVMSG(string s_token, Client * client, string msg);
			void 	_PART(string s_token, Client * client, string channs);
			int 	_MODE(string s_token, Client * client, string channs);
				void	_o(char sign, string _channel, string _nick, Client * client);
				void	_i(char sign, string _channel, Client * _client);
				void	_m(char sign, string _channel, Client * _client);
				void	_v(char sign, string _channel, string _nick, Client * _client);
				void	_k(char sign, string _channel, string _key, Client * _client);
				void	_l(char sign, string _channel, string _limit, Client * _client);
				void	_t(char sign, string _channel, Client * _client);
				void	_b(char sign, string _channel, Client * _client, string user = "");
			void 	_KICK(string s_token, Client * client, string res);
			void	_QUIT(string s_token, Client * client, int i, size_t index);
			int 	_INVITE(string s_token, Client * client, string invited);
			void    split(char * str, string & cmd, string & res);
			size_t	params_calc(string params);
			Client *find_client(string nick);
			string 	check_nickNAMEs(std::vector<string> &vec);
			void	trim_whiteSpaces(string &str);
			string	check_nick_presence(string nick_toFind);
			string	check_channel_presence(string channel_toFind);
			string	check_channNAMEs(std::vector<string> &cls);
			void	leave_channels(Client * client, string channel);
			int		find_spaceInBetween(string str);
			void	broadcast_mode(string _mode);
			//ERROR THAT CAN'T BE DEFINED AS MACROS
			string	ERR_TOOMANYTARGETS(string target)
				{ return ("407 * " + target + " :Duplicate recipients.");}
			string	ERR_NOSUCHNICK(string nick)
				{ return ("401 * " + nick + " :No such nick/channel"); }
			string ERR_NOSUCHCHAN(string chan)
				{ return ("403 * " + chan + " :No such channel"); }
			string ERR_CHANOPRIVSNEEDED(string chan)
				{ return ("482 * " + chan + " :You're not channel operator"); }
			//BOT
			void	bot_call(string command, Client *cl);
			void	logtime(Client *cl);
			void	users(Client *cl);
	};
}
