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
	else if (cmd == "help" || cmd == "HELP")
		help(cl);
	else if (cmd == "owners" || cmd == "OWNERS")
		owners(cl);
}

void	Server::help(Client *cl)
{
	string msg = ":Rαɠɳαɾöƙ NOTICE :\r\n" 
	"NOTICE Rαɠɳαɾöƙ :                          .-\"\"\"-.\r\n"
    "NOTICE Rαɠɳαɾöƙ :                         /`       `\"    \r\n"
    "NOTICE Rαɠɳαɾöƙ :  ,-==-.    b            ;           ;\r\n"
    "NOTICE Rαɠɳαɾöƙ : /(    \\`.              |           |\r\n"
   "NOTICE Rαɠɳαɾöƙ : |\\ ,-. \\ (             :           ;\r\n"
    "NOTICE Rαɠɳαɾöƙ : \\ \\`-.> ) 1             \\         /\r\n"
    "NOTICE Rαɠɳαɾöƙ :  \\_`.   | |              `._   _.`\r\n"
    "NOTICE Rαɠɳαɾöƙ :   \\o_`-_|/                _|`\"'|-.\r\n"
    "NOTICE Rαɠɳαɾöƙ :  /`  `>.  __          .-'`-|___|_ )    logtime\r\n"
    "NOTICE Rαɠɳαɾöƙ : |\\  (^  >'  `>-----._/             )   users\r\n"
    "NOTICE Rαɠɳαɾöƙ : | `._\\ /    /      / |      ---   -;    list\r\n"
    "NOTICE Rαɠɳαɾöƙ : :     `|   (      (  |      ___  _/     whois\r\n"
    "NOTICE Rαɠɳαɾöƙ :  \\     `.  `\\      \\_\\      ___ _/      help\r\n"
    "NOTICE Rαɠɳαɾöƙ :   `.     `-='`t----'  `--.______/       owners\r\n"
    "NOTICE Rαɠɳαɾöƙ :     `.   ,-''-.)           |---|       \r\n"
    "NOTICE Rαɠɳαɾöƙ :       `.(,-=-./             \\_/        (asalek & tnamir)\r\n"
    "NOTICE Rαɠɳαɾöƙ :          |   |               V\r\n"
    "NOTICE Rαɠɳαɾöƙ :          |-''`-.             `.\r\n"
	"NOTICE Rαɠɳαɾöƙ :       /  ,-'-.\\              `-.\r\n"
    "NOTICE Rαɠɳαɾöƙ :        |  (      \\                `.\r\n"
    "NOTICE Rαɠɳαɾöƙ :         \\  \\     |               ,.'\r\n"
	;
	this->send_msg(cl, msg);
}

void	Server::owners(Client *cl)
{
	string format = ":Rαɠɳαɾöƙ NOTICE :\r\n"
	"NOTICE Rαɠɳαɾöƙ :   ____                               \\ \r\n"
	"NOTICE Rαɠɳαɾöƙ :  /  __\\          ____                     \\ \r\n"
	"NOTICE Rαɠɳαɾöƙ :  \\( oo          (___ \\                     \\ \r\n"
	"NOTICE Rαɠɳαɾöƙ :  _\\_o/           oo~)/\r \n"
	"NOTICE Rαɠɳαɾöƙ :  / \\|/ \\         _\\-_/_ \r\n"
	"NOTICE Rαɠɳαɾöƙ : / / __\\ \\___    / \\|/  \\ \r\n"
	"NOTICE Rαɠɳαɾöƙ : \\ \\|   |__/_)  / / .- \\ \\ \r\n"
	"NOTICE Rαɠɳαɾöƙ :  \\/_)  |       \\ \\ .  /_/\r\n"
	"NOTICE Rαɠɳαɾöƙ :  ||___|        \\/___(_/\r\n"
	"NOTICE Rαɠɳαɾöƙ :  | | |          | |  |\r\n"
	"NOTICE Rαɠɳαɾöƙ :  | | |          | |  |\r\n"
	"NOTICE Rαɠɳαɾöƙ :  |_|_|          |_|__|\r\n"
	"NOTICE Rαɠɳαɾöƙ :  [__)_)        (_(___]\r\n"
	"NOTICE Rαɠɳαɾöƙ :  Ayoub Salek     Taha Namir\r\n";
	this->send_msg(cl, format);
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