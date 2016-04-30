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
    char *port = malloc(256); //port number
    char *host = malloc(256); //host address or name
    char *name = malloc(11);
    int errno;

    //Get port number and host name
    
    printf("Please enter the host to connect to: ");
    scanf("%s", host);
    
    printf("Please enter the port to connect to: ");
    scanf("%s", port);


    int goodName = -1;
    while(goodName == -1){
        printf("Please enter your name (Keep it under 10 characters): ");
        scanf("%s", name);
        int i = 0;
        while(name[i] != '\0'){
            i++;
        }
        if(i > 10){
            printf("Your name is too long, try again\n");
        }
        else goodName = 1;
    }
    
    
    // Get address info

    //Load up hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(host, port, &hints, &servinfo);

    if(status != 0){
        printf("Error with getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    //Get file descriptor
    int server;
    server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if(server == -1){
         printf("Error getting the file descripter: %i\n", errno);
    }


    //Connect

    if(connect(server, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
        printf("Error connecting: %i\n", errno);
        return -1;
    }
    
    //Start chat
    char buffer[513]; //500 plus max name size (10) and "> " size
    char message[500];
    char *endCase = "\\quit";
    strcpy(buffer, name);
    strcat(buffer, "> ");
    strcat(buffer, "Hello, I have connected!");
    
    int bytesSent = send(server, buffer, 513, 0);

    if( bytesSent == -1){
        printf("Error sending message: %i\n", errno);
        return -1;
    }
    printf("Waiting for first reply from Server\n");
    
    char c;
    while ((c = getchar()) != '\n' && c != EOF);  //Fix for newline problems in the buffer stream
    
    int end = -1;
    int reply = 1; 

    while(end == -1){
        
        reply = recv(server, buffer, 513, 0);
        if(reply != 0){
            printf("%s\n", buffer);
            
            printf("%s> ", name);

            fgets(message, sizeof(message), stdin);

            strtok(message, "\n");
            printf("Message:  !%s", message);
            if(strncmp(message, endCase, 5) == 0){
                printf("Exiting...\n");
                end = 1;
            }
            else{
                buffer[0] = '\0';
                strcpy(buffer, name);
                strcat(buffer, "> ");
                strcat(buffer, message);
                bytesSent = send(server, buffer, 513, 0);

                if( bytesSent == -1){
                    //cout << "Error sending message: " << errno << endl;
                    return -1;
                }
            }
        }
        else{
            printf("Connection has been closed by the server\n");
            end = 1;
        }

       

    }

    shutdown(server, 2);
    freeaddrinfo(servinfo); //free this at end
    return 0;
}
