#include "server.hpp"
#include <vector>

int fatal_error(std::string msg){
	std::cerr << "error: " << msg << std::endl;
	return EXIT_FAILURE;
}

int main(int argc, char **argv){

	if (argc != 2) return 1;

	int server_socket, port_nbr;

	std::vector<int> clients_sockets;

	char buffer[255];

	struct sockaddr_in server_addr, client_addr;

	socklen_t client_size;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
						//  IPV4,   TCP ,     tcp default
	if (server_socket < 0) return (fatal_error("socket failure"));
	
	bzero((char *) &server_addr, sizeof(server_addr));

	port_nbr = std::atoi(argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port_nbr);	 //host to network short. The htons() function converts the unsigned short integer port_nbr from host byte order to network byte order.
;
	if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
		return (fatal_error("bind failure"));
	if (listen(server_socket, 5) < 0)	//server_socket got detected only if a new member had joined
		return (fatal_error("listen failure"));

	fd_set curr_sock, ready_sock;
	FD_ZERO(&curr_sock);	//initialize the struct with NULL
	FD_SET(server_socket, &curr_sock);	//dup2 server_socket

	while (1)
	{
		ready_sock = curr_sock;	//save the file descriptor cause select is destructive
				//maximum_FD's, read_fd,write_fd,errors, timeout
		if (select(FD_SETSIZE, &ready_sock, 0, 0, 0) < 0)
			return fatal_error("select failed to manage file descriptor");
		for (int i = 0; i < FD_SETSIZE; i++){
			if (FD_ISSET(i, &ready_sock))//check if the fd is set by select
			{
				if (i == server_socket)//check if fd 'i' is our socket fd that's telling us that there's a new connection to accept
				{
					client_size = sizeof(client_addr);
					int acc = accept(server_socket, (struct sockaddr *) &client_addr, &client_size);
					if (acc < 0)
						return (fatal_error("accept failure"));
					FD_SET(acc, &curr_sock);
					clients_sockets.push_back(acc);//push the new Client_FD to vector
				}
				else
				{
					bzero(buffer, 255);
					int n;
					if ((n = read(clients_sockets[i - 4], buffer, 255)) < 0)
						return fatal_error("Failed to read.");
					std::cout << buffer << std::flush;
					if (!strncmp(buffer, "bye", 3))
					{
						// for (std::vector<int>::iterator it = clients_sockets.begin(); it != clients_sockets.end(); it++)
						// 	if (*it == clients_sockets[i - 4])
						clients_sockets.pop_back();
						FD_CLR(i, &curr_sock);
					}
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