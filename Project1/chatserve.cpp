/*William Maillard
CS 372-400
Project 1: chatserve
5/1/16
Description: This is the server side of a chat application that allows a client and 
server to communicate via the commandline.  More information about set-up, and command-line
arguments can be found in the README.txt file.
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
int startUp(const char *port);
int acceptConnection(int server);
int receiveMessage(int client, char* buffer, int maxBuff);
int sendMessage(int client);

int main(int argc, char *argv[])								//argv[1] is the port number
{
    int errno;
	int client;
	const char* port = argv[1];
	
																//Check number of command line arguments
	if(argc != 2){
		printf("Please follow this format: chatserve <port number>\n");
		return -1;
	}
																//Most of the set up and chat interface come from Beej's guide
																//http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html

	printf("Connecting on port %s...\n", port);
	int server = startUp(port);									//Start listening on port
	if(server == -1) return -1;
	
	

	while(true){												//Keep listening open even after chat is done (until SIGINT)
		
		printf("Waiting for a connection from the client on port, %s...\n", port);
		client = acceptConnection(server);
		if(client == -1) return -1;

		const char *name = "Server";
		int maxBuff = 515;										//500 plus max name size (10) and "> " size
		
		//Start chat
		char buffer[maxBuff]; 

		while(true){											//Continue the chat until broken out (client or server end chat with \quit)
			
									
			if(receiveMessage(client, buffer, maxBuff) == -1){	//Receive first message 
				break;
			}
			printf("%s\n", buffer);								//Print received message
			
			int sent = sendMessage(client);						//Send message, break if "\quit", return if error
			if(sent == 1){
				break;
			}
			else if(sent == -1){
				return -1;
			}
		}

    	if(shutdown(client, 2) != 0){
			printf("Error shutting down connection: %d", errno);
		}


	} //end main while
	

    return 0;
}

//Function: startUp
//Arguments: The port number to start a connection on
//Starts a connection on that port and returns the server number or
// -1 if startup failed (plus an error message is sent to stdout)

int startUp(const char *port){
	
	int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
	
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
	return server;
}

//Function: acceptConnection
//Arguments: The server number
//Accepts a connection for the server and returns the client number or
// -1 if startup failed (plus an error message is sent to stdout)

int acceptConnection(int server){
	
	struct sockaddr_storage clientAddr;
	int client;
	socklen_t addrSize = sizeof clientAddr;

	client = accept(server, (struct sockaddr *) &clientAddr, &addrSize);		//Accept connection

	if( client == -1 ){
		 cout << "Error accepting connection: " << errno << endl;
		 return -1;
	}
	return client;
}

//Function: receiveMessage
//Arguments: client number, a buffer, the buffer's size
//Receives a message and puts it in buffer.
//Returns -1 if the connections has been closed and 0 on success.

int receiveMessage(int client, char* buffer, int maxBuff){
	
		if(recv(client, buffer, maxBuff, 0) != 0){
			return 0;
		}
		else{
			printf("Connection has been closed by the Client\n");
			return -1;
		}
}

//Function: receiveMessage
//Arguments: client number
//Receives a message and puts it in buffer.
//Returns -1 on error, 0 on success, and 1 on choosing to close the connection.

int sendMessage(int client){
			
	char message[501];
	char *endCase = "\\quit";
	char *name = "Server";
	int maxBuff = 515;
	char buffer[maxBuff];
	
	printf("%s> ", name);
	fgets(message, sizeof(message), stdin);					//Get the message from the user
	strtok(message, "\n");

	if(strncmp(message, endCase, 5) == 0){					//If server types \quit, close connection
		printf("Closing the connection with the client...\n");
		return 1;
	}
	else{
		buffer[0] = '\0';
		strcpy(buffer, name);
		strcat(buffer, "> ");
		strcat(buffer, message);

		if( send(client, buffer, maxBuff, 0) == -1){
			cout << "Error sending message: " << errno << endl;
			return -1;
		}
	}
	return 0;
}

	



