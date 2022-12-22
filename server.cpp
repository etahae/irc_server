#include "server.hpp"

int main(int argc, char **argv){

    if (argc != 2) return 1;

    (void)argc;

    int sockfd, newsockfd, port_nbr;

    char buffer[255];

    struct sockaddr_in server_addr, client_addr;

    socklen_t client_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return 1;
    
    bzero((char *) &server_addr, sizeof(server_addr));

    port_nbr = std::atoi(argv[1]);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_nbr);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        return 1;

    if (listen(sockfd, 5) < 0)
        return 1;

    client_size = sizeof(client_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_size);
    if (newsockfd < 0) return 1;

    while (1){
        if (read(newsockfd, buffer, 255) < 0) return 1;
        std::cout << buffer << std::flush;
        std::cin >> buffer;
        std::string buffeer = buffer;
        buffeer += "\n";
        if (write(newsockfd, buffeer.c_str(), strlen(buffeer.c_str())) < 0)
            return 1;
    }

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