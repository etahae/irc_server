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