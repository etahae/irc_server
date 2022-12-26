#include "server.hpp"

int Server::fatal_error(std::string msg){
	std::cerr << "error: " << msg << std::endl;
	return EXIT_FAILURE;
}

int Server::init_server(char **argv){
	this->password = argv[2];
	this->port_number = std::atoi(argv[1]);
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
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
