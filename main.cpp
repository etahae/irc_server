#include "server.hpp"


int main(int argc, char **argv){

	Server server;
	Client client;

	if (argc != 3)
		return (server.fatal_error("Usage: $> ./ircserv <port> <password>"));
	if (server.init_server(argv))
		return EXIT_FAILURE;

	struct sockaddr_in client_addr;
	socklen_t client_size;

	fd_set _socket, tmp_socket, w_socket, tmp_w_socket;
	FD_ZERO(&_socket);
	FD_SET(server._socket, &_socket);

	char buffer[ARG_MAX];

	while (1){
		tmp_socket = _socket;
		// tmp_w_socket = w_socket;
		if (select(FD_SETSIZE, &tmp_socket, 0, 0, 0) < 0) return 1;
		for (int i = 0; i < FD_SETSIZE; i++){
			if (FD_ISSET(i, &tmp_socket)){
				if (i == server._socket){
					client_size = sizeof(client_addr);
					int acc = accept(server._socket, (struct sockaddr *) &client_addr, &client_size);
					if (acc < 0) return (server.fatal_error("accept failure"));
					FD_SET(acc, &_socket);
					server.clients.push_back(Client(acc, client_addr));
					write(acc, WELCOME_MSG, strlen(WELCOME_MSG));
				}
				else{
					bzero(buffer, 255);
					int n;
					if ((n = read(server.clients[i - 4].fd_socket, buffer, ARG_MAX)) < 0) return 1;
					std::cout << buffer << std::flush;
					if (!strncmp(buffer, "bye", 3)){
						server.clients.pop_back();
						FD_CLR(i, &_socket);
					}
					// std::cin >> buffer;
					// for (size_t i = 0; i < server.clients.size(); i++)
					// {
					// 	write(server.clients[i].fd_socket, "buffer", 7);
					// }
				}
			}
		}
		// bzero(buffer, 255);
		// if (read(newserver_socket, buffer, 255) < 0) return 1;
		// std::cout << buffer;
		// bzero(buffer, 255);
		// fgets(buffer, 255, stdin);
		// if (write(newserver_socket, buffer, strlen(buffer)) < 0)
		//     return 1;
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