#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <cstring>

using std::string;
using std::cout;
using std::endl;

class Server{
    public :

        string         password;
        int                 port_number;
        int                 _socket;
        std::vector<int>    clients_sockets;
        struct sockaddr_in  server_addr;


        int fatal_error(string msg);
        int init_server(char **argv);
};
