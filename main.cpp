/*
	TCP / IP Model :
		1_Application Layer :
			youtube, google, ... this layer contains protocols,
			such as: HTTP on internet web sites and SMTP email check
			or FTP to transfer files
		2_Transport Layer :
			data become small piece of packages and send them to distination
			then get gathered and check if are all there
		3_Internet Layer :
			uses the internet protocol (IP) to know the sender and reciever
		4_Network Layer :
			uses macAddress to reach the exact machine
*/
#include "server.hpp"

using namespace irc;

bool is_digits_or_length_over(char *str)
{
	int i = 0;

	// if (str.find_first_not_of("0123456789") == std::string::npos)
	// 	return true;
	if (strlen(str) > 5)
		return true;
	while (str[i])
	{
		if (isdigit(str[i]))
			i++;
		else
			return true;
	}
	if (atoi(str) > 65535 || atoi(str) < 1024)
		return true;
	return false;
}

int	findClientIndexByFD(Server &server, int fd)
{
	for(size_t i = 0; i < server.clients.size(); i++)
		if (server.clients[i]->fd_socket == fd)
			return i;
	return -1;
}

void connect(Server &server, char *buffer, int i, size_t index)
{
	std::cout << buffer << std::flush;
	size_t char_index = 0;
	string str_buffer;
	while (buffer[char_index])
	{
		while (buffer[char_index] != '\n' && buffer[char_index])
		{
			str_buffer += buffer[char_index];
			char_index++;
		}
		str_buffer += buffer[char_index];
		char_index++;
		if (server.clients[index]->verified == false)
		{
			if (server.client_verifying(const_cast<char *>(str_buffer.c_str()), server.clients[index], i, index))
			{
				if (server.clients[index]->verified == false)
				{
					std::string rpl = ":Rαɠɳαɾöƙ 001 " + server.clients[index]->nick + " : welcome to the internet relay chat\r\n"
					":Rαɠɳαɾöƙ 002 " + server.clients[index]->nick + " :Your host is Rαɠɳαɾöƙ, running version 1.0\r\n"
					":Rαɠɳαɾöƙ 003 " + server.clients[index]->nick + " :This server was created 28/12/2022\r\n"
					":Rαɠɳαɾöƙ 004 " + server.clients[index]->nick + " Rαɠɳαɾöƙ 1.0 - -\r\n"
					":Rαɠɳαɾöƙ 372 " + server.clients[index]->nick + " 🔨 𝔚𝔢𝔩𝔠𝔬𝔪𝔢 𝔗𝔬 Rαɠɳαɾöƙ 🔨\r\n";
					write(server.clients[index]->fd_socket, rpl.c_str(), rpl.size());
					server.clients[index]->verified = true;
					cout << CONNECTED << server.clients[index]->nick << " Connected" << endl;
				}
			}
		}
		else
			server.customer_service(const_cast<char *>(str_buffer.c_str()), server.clients[index], i, index);
		str_buffer = "";
	}
	if (server.clients[index]->verified == true)
		write(server.clients[index]->fd_socket, VERIFIED, strlen(VERIFIED));
}

int main(int argc, char **argv)
{

	Server server;

	if (argc != 3)
		return (server.fatal_error("Usage: $> ./ircserv <port> <password>"));
	if (is_digits_or_length_over(argv[1]))
		return (server.fatal_error("ircServer: port not valid"));
	if (server.init_server(argv))
		return EXIT_FAILURE;
	struct sockaddr_in client_addr;
	socklen_t client_size;

	fd_set tmp_r_socket, tmp_w_socket;
	FD_ZERO(&server.r_socket);
	FD_ZERO(&server.w_socket);
	FD_SET(server._socket, &server.r_socket); // this to read from client sockets and accept them

	char buffer[ARG_MAX];

	while (1)
	{
		tmp_r_socket = server.r_socket;
		tmp_w_socket = server.w_socket;
		if (select(FD_SETSIZE, &tmp_r_socket, &tmp_w_socket, 0, 0) < 0)
			return 1;
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &tmp_r_socket))
			{
				if (i == server._socket)
				{
					client_size = sizeof(client_addr);
					int acc = accept(server._socket, (struct sockaddr *)&client_addr, &client_size);
					if (acc < 0)
						return (server.fatal_error("accept failure"));
					FD_SET(acc, &server.r_socket);
					FD_SET(acc, &server.w_socket);
					server.clients.push_back(new Client(acc, client_addr, inet_ntoa(client_addr.sin_addr)));
				}
				else
				{
					int	client_index = findClientIndexByFD(server, i);
					bzero(buffer, ARG_MAX);
					int n;
					if (client_index >= 0)
					{
						if ((n = read(server.clients[client_index]->fd_socket, buffer, ARG_MAX)) < 0)
							return 1;
						if (n > 0)
							connect(server, buffer, i, client_index);
						if (n == 0)
							server.disconnect(client_index, i, server.clients[client_index]->fd_socket);
					}
				}
			}
		}
	}
	return (0);
}

// socket function returns int server_socket : takes three parameters :
//  first one : int domain : AF_INET (ipv4)
//  second one : int type : sock_stream (TCP)
//  third one : int protocol : 0 (default tcp protocol)

// bind function returns 0 succes , -1 failure : take three parameters :
//  first one : int server_socket
//  second one : struct sockaddr *
//  third one :  socklen_t (size of sockaddr)

// struct sockaddr { sa_family_t sa_family ;
//                   char sa_data[14]; }

// listen function returns 0 succes , -1 failure : takes two parameters :
//  first one : int server_socket
// second one : the max connections allowed in the server

// accept function returns : take three parameters :
//  first one : int server_socket
//  second one : (struct sockaddr *) & addr
//  third  one : & socklen
//  (waits for the connection function in the client to connect)