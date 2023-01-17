#include "server.hpp"
using namespace irc;
using std::to_string;

void	Server::bot_call(std::string command, Client *cl)
{
	std::string cmd = command.substr(1, command.size());
	this->trim_whiteSpaces(cmd);
	if(cmd == "logtime" || cmd == "LOGTIME")
		logtime(cl);
	else if (cmd == "users" || cmd == "USERS")
		users(cl);
	else if (cmd == "list" || cmd == "LIST")
		channels_list(cl);
	else if (cmd.compare(0, 5, "whois") == 0 || cmd.compare(0, 5, "WHOIS") == 0)
		whois(cl, cmd);
}

void	Server::logtime(Client *cl)
{
	tm		*ltm;
	time_t now = time(0);
	ltm = localtime(&now);
	long	time_loged_second = (time(NULL) - cl->start);

	int minute = time_loged_second / 60;
	int seconds = time_loged_second % 60;
	int hour = minute / 60;
	minute = minute % 60;

	string format = "300 * " + (hour > 9 ? to_string(hour) : ("0" + to_string(hour)))
	 + ":" + (minute > 9 ? to_string(minute) : ("0" + to_string(minute)))
	 + ":" + (seconds > 9 ? to_string(seconds) : ("0" + to_string(seconds)));
	this->send_msg(cl, format);
}

void	Server::users(Client *cl)
{
	string msg = "300 * " + to_string(this->clients.size()) + " active user.";
	this->send_msg(cl, msg);
}

void	Server::channels_list(Client *cl)
{
	if (this->channels.size() == 0)
		this->send_msg(cl, "300 * NO SUCH CHANNEL IN THE SERVER YET");
	else
		for (std::map<string, Channel *>::iterator it = this->channels.begin(); it != this->channels.end(); it++)
			this->send_msg(cl, "300 * " + it->first);
}

void	Server::whois(Client *cl, string cmd)
{
	trim_whiteSpaces(cmd);
	string whois_ = "";
	if (cmd.find_first_of(' ') == string::npos)
		return ;
	whois_ = cmd.substr(cmd.find_first_of(' ') + 1, cmd.size());
	cout << "*" << whois_ << "*\n";
	size_t i = 0;
	if (whois_ == "")
		return ;
	for (;i < clients.size(); i++)
	{
		if (clients[i]->nick == whois_)
			break ;
	}
	if (i < clients.size())
		this->send_msg(cl, "300 * " + clients[i]->user_info());
}