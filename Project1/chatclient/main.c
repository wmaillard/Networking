#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>


int main()
{
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    const char *port = "4351"; //port number
    int errno;


    // Get address info

    //Load up hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo("127.0.0.1", port, &hints, &servinfo);

    if(status != 0){
        //cout << "getaddrinfo error: " << gai_strerror(status) <<  endl;
        return -1;
    }

    //Get file descriptor
    int server;
    server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if(server == -1){
         //cout << "Error getting the file descripter: " << errno << endl;
    }


    //Bind socket, don't need for client
   /* if( bind(client, servinfo->ai_addr, servinfo->ai_addrlen) == -1 ){
         //cout << "Error binding socket: " << errno << endl;
    }*/


    //Connect

    if(connect(server, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
        printf("Error connecting: %i", errno);
    }



    char buffer[500];


    recv(server, buffer, 500, 0);
    printf("%s", buffer);



    strcpy(buffer, "Client is here!\n");

    int bytesSent = send(server, buffer, 500, 0);

    if( bytesSent == -1){
        //cout << "Error sending message: " << errno << endl;
        return -1;
    }


/*
    //Accept connection
    struct sockaddr_storage clientAddr;
    int server;
    socklen_t addrSize = sizeof clientAddr;

    //cout << "Waiting for a connection from the client..." << endl;
    client = accept(server, (struct sockaddr *) &clientAddr, &addrSize);

    if( client == -1 ){
         //cout << "Error accepting connection: " << errno << endl;
    }
    else //cout << "Connected to: " << client << endl;
    //


*/

    freeaddrinfo(servinfo); //free this at end
    return 0;
}
