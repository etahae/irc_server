#pragma once

#include"client.hpp"

class Server{
    public :

        string         password;
        int                 port_number;
        int                 _socket;
        std::vector<Client>    clients_sockets;
        struct sockaddr_in  server_addr;


        int fatal_error(string msg);
        int init_server(char **argv);
};
