#include "server.hpp"

int main(int argc, char **argv){

    if (argc != 2) return 1;
    

    int sockfd, newsockfd, port_nbr;

    char buffer[255];

    struct sockaddr_in server_addr, client_addr;

    socklen_t client_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return 1;
    
    bzero((char *) &server_addr, sizeof(server_addr));

    port_nbr = std::atoi(argv[1]);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port_nbr);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        return 1;

    if (listen(sockfd, 10) < 0)
        return 1;

    client_size = sizeof(client_addr);

    fd_set curr_sock, ready_sock;
    FD_ZERO(&curr_sock);
    FD_SET(sockfd, &curr_sock);

    while (1){
        ready_sock = curr_sock;
         
        if (select(FD_SETSIZE, &ready_sock, 0, 0, 0) < 0) return 1;
        for (int i = 0; i < FD_SETSIZE; i++){
            if (FD_ISSET(i, &ready_sock)){
                if (i == sockfd){
                    newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_size);
                    if (newsockfd < 0) return 1;
                    FD_SET(newsockfd, &curr_sock);
                }
                else{
                    bzero(buffer, 255);
                    if (read(newsockfd, buffer, 255) < 0) return 1;
                    std::cout << buffer;
                    FD_CLR(i, &curr_sock);
                }
            }
        }
        // bzero(buffer, 255);
        // if (read(newsockfd, buffer, 255) < 0) return 1;
        // std::cout << buffer;
        // bzero(buffer, 255);
        // fgets(buffer, 255, stdin);
        // if (write(newsockfd, buffer, strlen(buffer)) < 0)
        //     return 1;
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