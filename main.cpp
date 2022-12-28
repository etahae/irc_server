#include "server.hpp"


int main(int argc, char **argv){

	Server server;

	if (argc != 3)
		return (server.fatal_error("Usage: $> ./ircserv <port> <password>"));
	if (server.init_server(argv))
		return EXIT_FAILURE;

	struct sockaddr_in client_addr;
	socklen_t client_size;

	fd_set _socket, tmp_socket, w_socket, tmp_w_socket;
	FD_ZERO(&_socket);
	FD_SET(server._socket, &_socket);	//this to read from client sockets and accept them

	char buffer[ARG_MAX];

	while (1)
	{
		tmp_socket = _socket;
		tmp_w_socket = w_socket;
		if (select(FD_SETSIZE, &tmp_socket, &tmp_w_socket, 0, 0) < 0)
			return 1;
		for (int i = 0; i < FD_SETSIZE; i++){
			if (FD_ISSET(i, &tmp_socket))
			{
				if (i == server._socket)
				{
					client_size = sizeof(client_addr);
					int acc = accept(server._socket, (struct sockaddr *) &client_addr, &client_size);
					if (acc < 0) return (server.fatal_error("accept failure"));
					FD_SET(acc, &_socket);
					FD_SET(acc, &w_socket);
					server.clients.push_back(Client(acc, client_addr));
				}
				else
				{
					bzero(buffer, 255);
					int n;
					if ((n = read(server.clients[i - 4].fd_socket, buffer, ARG_MAX)) < 0)
						return 1;
					if (n > 0)
					{
						std::cout << buffer << std::flush;
						if (server.clients[i - 4].verified == false)
						{
							if (server.client_verifying(buffer, &server.clients[i - 4]))
							{
								if (server.clients[i - 4].verified == false)
								{
									std::string rpl = ": 001 " + server.clients[i - 4].nick + " :        🔨 Wҽʅƈσɱҽ TӨ Rαɠɳαɾöƙ 🔨\r\n";
									write(server.clients[i - 4].fd_socket, rpl.c_str(), rpl.size());
									server.clients[i - 4].verified = true;
								}
							}
						}
						if (server.clients[i - 4].verified == true)
							write(server.clients[i - 4].fd_socket, VERIFIED, strlen(VERIFIED));
					}
					if (n == 0)
					{
						cout << DISCONNECTED << server.clients[i - 4].nick << " Disconnected" << endl;
						FD_CLR(i, &_socket);
						FD_CLR(i, &w_socket);
						close(server.clients[i - 4].fd_socket);
						server.clients.erase(server.clients.begin() + i - 4);
					}
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