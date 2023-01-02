#include "server.hpp"

using namespace irc;

bool	is_digits_or_length_over(char *str)
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

int main(int argc, char **argv){

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
	FD_SET(server._socket, &server.r_socket);	//this to read from client sockets and accept them

	char buffer[ARG_MAX];

	while (1)
	{
		tmp_r_socket = server.r_socket;
		tmp_w_socket = server.w_socket;
		if (select(FD_SETSIZE, &tmp_r_socket, &tmp_w_socket, 0, 0) < 0)
			return 1;
		for (int i = 0; i < FD_SETSIZE; i++){
			if (FD_ISSET(i, &tmp_r_socket))
			{
				if (i == server._socket)
				{
					client_size = sizeof(client_addr);
					int acc = accept(server._socket, (struct sockaddr *) &client_addr, &client_size);
					if (acc < 0) return (server.fatal_error("accept failure"));
					FD_SET(acc, &server.r_socket);
					FD_SET(acc, &server.w_socket);
					server.clients.push_back(new Client(acc, client_addr));
					// (server.clients->end() - 1)->ip_address = inet_ntoa(client_addr.sin_addr);
				}
				else
				{
					bzero(buffer, ARG_MAX);
					int n;
					if ((n = read(server.clients[i - 4]->fd_socket, buffer, ARG_MAX)) < 0)
						return 1;
					if (n > 0)
					{
						std::cout << buffer << std::flush;
						if (server.clients[i - 4]->verified == false)
						{
							if (server.client_verifying(buffer, server.clients[i - 4]))
							{
								if (server.clients[i - 4]->verified == false)
								{
									std::string rpl = ":Rαɠɳαɾöƙ 001 " + server.clients[i - 4]->nick + " :        🔨 𝔚𝔢𝔩𝔠𝔬𝔪𝔢 𝔗𝔬 ℑ𝔫𝔱𝔢𝔯𝔫𝔢𝔱 ℜ𝔢𝔩𝔞𝔶 ℭ𝔥𝔞𝔱 🔨\r\n"
									":Rαɠɳαɾöƙ 002 " + server.clients[i - 4]->nick + " :Your host is Rαɠɳαɾöƙ, running version 1.0\r\n"
									":Rαɠɳαɾöƙ 003 " + server.clients[i - 4]->nick + " :This server was created 28/12/2022\r\n";
									write(server.clients[i - 4]->fd_socket, rpl.c_str(), rpl.size());
									server.clients[i - 4]->verified = true;
									cout << CONNECTED << server.clients[i - 4]->nick << " Connected" << endl;
									// return (":server NOTICE taha : JOJO\r\n");
								}
							}
						}
						if (server.clients[i - 4]->verified == true)
							write(server.clients[i - 4]->fd_socket, VERIFIED, strlen(VERIFIED));
					}
					if (n == 0)
						server.disconnect(i, server.clients[i - 4]->fd_socket);
				}
			}
		}
	}
	return (0);
}

//socket function returns int server_socket : takes three parameters : 
	// first one : int domain : AF_INET (ipv4)
	// second one : int type : sock_stream (TCP)
	// third one : int protocol : 0 (default tcp protocol)

//bind function returns 0 succes , -1 failure : take three parameters :
	// first one : int server_socket
	// second one : struct sockaddr *
	// third one :  socklen_t (size of sockaddr)

// struct sockaddr { sa_family_t sa_family ; 
//                   char sa_data[14]; }

//listen function returns 0 succes , -1 failure : takes two parameters :
	// first one : int server_socket
	//second one : the max connections allowed in the server

//accept function returns : take three parameters :
	// first one : int server_socket
	// second one : (struct sockaddr *) & addr
	// third  one : & socklen
	// (waits for the connection function in the client to connect)