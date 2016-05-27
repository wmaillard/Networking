/*William Maillard
CS 372-400
Project 1
5/1/16
Description: This is the client side of a chat application that allows a client and 
server to communicate via the commandline.  More information about set-up, and command-line
arguments can be found in the README.txt file.
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

int initiateContact(char *name, char* host, char* port);
int receiveMessage(char *name, int server);

int main(int argc, char *argv[])
{

    char *port = argv[2];					//port number
    char *host = argv[1]; 					//host address or name
    char *name = malloc(11);
    int errno;
	
											//Check number of command line arguments
	if(argc != 3){
		printf("Please follow this format: chatclient <host name> <port number>\n");
		return -1;
	}


    int goodName = -1;						//Get username and make sure it is under 10 characters
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
    
    //Most of the set up and chat interface come from Beej's guide http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
	int server = initiateContact(name, host, port);
	if(server == -1){
		return -1;
	}
	printf("Waiting for first reply from Server\n");
    
    //Start chat
	int maxBuff = 515;   //501 plus max name size (11) and "> " size
    char buffer[maxBuff]; 

    char *endCase = "\\quit";
	
	
	//Start chat and continue until break (\quit from client or server)
    while(1 == 1){
        
		if(receiveMessage(name, server) == -1){						
			printf("Connection has been closed by the server\n");
			break;
		}
		int messageResult = sendMessage(name, server);
		if(messageResult == -1){
			printf("Exiting...\n");
			break;
        }
		else if(messageResult == 0){
			return -1;
		}

	}   

    if(shutdown(server, 2) != 0){
		printf("Error shutting down connection: %d", errno);
	}
   
    return 0;
}

//Function: initiateContact
//Arguments: The username, the host, and the port
//Gets first message from user and sends it to the server
//Returns -1 on failure or the server number on success.

int initiateContact(char *name, char* host, char* port){
	int status;
	struct addrinfo hints;
    struct addrinfo *servinfo;
	int c;
	// Get address info

    //Load up hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
	char message[501];
	char buffer[515];
	int maxBuff = 515;

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
	
	printf("Enter your first message to send:\n%s> ", name);		//Get initial contact message from user
	
	while ((c = getchar()) != '\n' && c != EOF);		  			//Fix for newline problems in the buffer stream from 
																	//here:http://ubuntuforums.org/archive/index.php/t-1059917.html 
	fgets(message, sizeof(message), stdin);
	strtok(message, "\n");
	
	strcpy(buffer, name);												
    strcat(buffer, "> ");
    strcat(buffer, message);
    
    if(send(server, buffer, maxBuff, 0) == -1){
        printf("Error sending message: %i\n", errno);
        return -1;
    }
	return server;
    
}

//Function: receiveMessage
//Arguments: The username and the server number
//Receives a message from the server and prints it
//with the username prepended.
//Returns 1 on success, -1 on connections closure

int receiveMessage(char *name, int server){
	int maxBuff = 515;
	char buffer[maxBuff];
	
	if(recv(server, buffer, maxBuff, 0) != 0){
		printf("%s\n", buffer);
		printf("%s> ", name);
		return 1;
	}
	else return -1;
}

//Function: receiveMessage
//Arguments: The username and the server number
//Get a message to send, prepend username, and send it
//Returns 1 on success, 0 on error, and -1 on connection closure

int sendMessage(char *name, int server){
	int errno;
	char message[501];
	int maxBuff = 515;
	char buffer[maxBuff];
	char *endCase = "\\quit";
	
	fgets(message, sizeof(message), stdin);			//Get the message to send

	strtok(message, "\n");
	if(strncmp(message, endCase, 5) == 0){			//If client types \quit, return -1
		return -1;
	}
	else{
		buffer[0] = '\0';
		strcpy(buffer, name);
		strcat(buffer, "> ");
		strcat(buffer, message);

		if(send(server, buffer, 5, 0) == -1){
			printf("Error sending message: %i", errno);
			return 0;
		}
	}
	return 1;
}
	
	
	