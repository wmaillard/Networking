#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/unistd.h>

using namespace std;

int main()
{
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
	string portString;
    int errno;
	int numPort = -1;
	const char *port;
	
	//Get Port number
	while(numPort <= 1024 || numPort > 65535){
		cout << "Please enter what port number to start on: ";
		cin >> portString;
		port = portString.c_str();
		numPort = atoi(port);
		if(numPort <= 1024 || numPort > 65535){
			cout << "Error, please enter a port greater than 1024 and less than 65536" << endl;
		}
	}

	
	
	cout << "Connecting on port " << portString << "..." << endl;

    // Get address info

    //Load up hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, port, &hints, &servinfo);

    if(status != 0){
        cout << "getaddrinfo error: " << gai_strerror(status) <<  endl;
        return -1;
    }

    //Get file descriptor
    int server;
    server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if(server == -1){
         cout << "Error getting the file descripter: " << errno << endl;
         return -1;
    }


    //Bind socket
    if( bind(server, servinfo->ai_addr, servinfo->ai_addrlen) == -1 ){
         cout << "Error binding socket: " << errno << endl;
         return -1;
    }


    //Start listening

    if( listen(server, 5) == -1 ){
         cout << "Error starting to listen: " << errno << endl;
         return -1;
    }


    //Accept connection
    struct sockaddr_storage clientAddr;
    int client;
    socklen_t addrSize = sizeof clientAddr;

    cout << "Waiting for a connection from the client on port " << portString << "..." << endl;
    client = accept(server, (struct sockaddr *) &clientAddr, &addrSize);

    if( client == -1 ){
         cout << "Error accepting connection: " << errno << endl;
         return -1;
    }


    const char *name = "Server";
	//Start chat
    char buffer[513]; //500 plus max name size (10) and "> " size
    char message[500];
	int bytesSent;
	int reply = 1; 
	char c;
	char *endCase = "\\quit";
	
	printf("endcase: %s", endCase);
	
	//Receive first message and clean input stream
	reply = recv(client, buffer, 513, 0);
	if(reply != 0){
		printf("%s\n", buffer);
	}
	else{
		printf("Error receiving first message\n");
		return -1;
	}
    while ((c = getchar()) != '\n' && c != EOF);  //Fix for newline problems in the buffer stream
	
	
	
    int end = -1;

    while(end == -1){
				
		printf("%s> ", name);
        fgets(message, sizeof(message), stdin);  //This is getting first word from first message from client
		
		strtok(message, "\n");
		
		
        if(strncmp(message, endCase, 5) == 0){
			printf("Exiting...\n");
            end = 1;
        }
        else{
			buffer[0] = '\0';
            strcpy(buffer, name);
            strcat(buffer, "> ");
            strcat(buffer, message);
            bytesSent = send(client, buffer, 513, 0);

            if( bytesSent == -1){
                //cout << "Error sending message: " << errno << endl;
                return -1;
            }
			
			reply = recv(client, buffer, 513, 0);
			if(reply != 0){
				printf("%s\n", buffer);
			}
			else{
				printf("Connection has been closed by the client\n");
				end = 1;
			}
        }
		




    }




    shutdown(server, 2);

    freeaddrinfo(servinfo); //free this at end
    return 0;
}
