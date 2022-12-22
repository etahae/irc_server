#include "server.hpp"

int main(int argc, char **argv){

    // if (argc != 3)
    //     return 1;
    (void)argc; (void)argv;

    

    return (0);
}

//socket function returns int sockfd : takes three parameters : 
    // first one : int domain : AF_INET (ipv4)
    // second one : int type : sock_stream (TCP)
    // third one : int protocol : 0 (default tcp protocol)

//bind function returns 0 succes , -1 failure : take three parameters :
    // first one : int sockfd
    // second one : struct sockaddr *
    // third one :  socklen_t (size of sockaddr)

// struct sockaddr { sa_family_t sa_family ; 
//                   char sa_data[14]; }

//listen function returns 0 succes , -1 failure : takes two parameters :
    // first one : int sockfd
    //second one : the max connections allowed in the server

//accept function returns : take three parameters :
    // first one : int sockfd
    // second one : (struct sockaddr *) & addr
    // third  one : & socklen
    // (waits for the connection function in the client to connect)