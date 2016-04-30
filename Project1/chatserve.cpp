/*William Maillard
CS 372-400
Project 1
5/1/16
*/


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

int main(int argc, char *argv[])
{
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;

    int errno;
	int numPort = -1;
	
	//Check number of command line arguments
	if(argc != 2){
		printf("Please follow this format: chatserve <port number>\n");
		return -1;
	}
	
	//Get Port number
	const char *port = argv[1];
	string portString = argv[1];
	
	cout << "Connecting on port " << portString << "..." << endl;
	
	
    //Most of the set up and chat interface come from Beej's guide http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html

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
	
	//Keep listening open even after chat is done (until SIGINT)
	while(true){
		
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
		int maxBuff = 515;//500 plus max name size (10) and "> " size
		
		//Start chat
		char buffer[maxBuff]; 
		char message[501];
		int bytesSent;
		int reply = 1; 
		char c;
		char *endCase = "\\quit";

		//Receive first message 
		reply = recv(client, buffer, maxBuff, 0);
		if(reply != 0){
			printf("%s\n", buffer);
		}
		else{
			printf("Error receiving first message\n");
			return -1;
		}

		int end = -1;  //Continue the chat until end is changed (client or server end chat with \quit)

		while(end == -1){

			printf("%s> ", name);
			fgets(message, sizeof(message), stdin);

			strtok(message, "\n");


			if(strncmp(message, endCase, 5) == 0){					//If server types \quit, close connection
				printf("Closing the connection with the client...\n");
				end = 1;
			}
			else{
				buffer[0] = '\0';
				strcpy(buffer, name);
				strcat(buffer, "> ");
				strcat(buffer, message);
				bytesSent = send(client, buffer, maxBuff, 0);

				if( bytesSent == -1){
					cout << "Error sending message: " << errno << endl;
					return -1;
				}

				reply = recv(client, buffer, maxBuff, 0);
				if(reply != 0){
					printf("%s\n", buffer);
				}
				else{												//If client types \quit, close connection
					printf("Connection has been closed by the client\n");
					end = 1;
				}
			}

		}
    	if(shutdown(client, 2) != 0){
			printf("Error shutting down connection: %d", errno);
		}


} //end main while
	
    freeaddrinfo(servinfo);  //free this at the end
    return 0;
}
