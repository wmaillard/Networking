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
#include <vector>
#include <fcntl.h>

using namespace std;
int startUp(const char *port);
int acceptConnection(int server);
int receiveMessage(int client, string *received);
int sendMessage(int client);
int sendFile(int client);

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
	
	string received;
	

	while(true){												//Keep listening open even after chat is done (until SIGINT)
		
		printf("Waiting for a connection from the client on port, %s...\n", port);
		client = acceptConnection(server);
		if(client == -1) return -1;

		const char *name = "Server";

		
		
																//Start chat

		while(true){											//Continue the chat until broken out (client or server end chat with \quit)
			
									
			if(receiveMessage(client, &received) == -1){	//Receive first message 
				break;
			}
			cout << received << endl;								//Print received message
			
			int sent = sendFile(client);						//Send message, break if "\quit", return if error
			break;
			if(sent == 1){
				break;
			}
			else if(sent == -1){
				return -1;
			}
		}

    	if(shutdown(client, 2) != 0){							//Shutdown connection
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

int receiveMessage(int client, string *received){
	
	int maxBuff = 4000;					//The idea of how to convert a buffer into a string came from here: http://stackoverflow.com/questions/18670807/sending-and-receiving-stdstring-over-socket		
	
	vector<char> buffer(maxBuff);
	int numBytes = 0;
	
	do{
		numBytes = recv(client, buffer.data(), buffer.size(), 0);
		if(numBytes != -1){
			string part = string(buffer.begin(), buffer.end());
			received->append(part);
		}
		else{
			printf("Connection has been closed by the client\n");
			return -1;
		}
	}while(numBytes == maxBuff);
	return 0;
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

int sendFile(int client){  //This is all from here: http://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
	int fp = open("test.txt", O_RDONLY);
	
	int maxBuff = 4000;					//The idea of how to convert a buffer into a string came from here: http://stackoverflow.com/questions/18670807/sending-and-receiving-stdstring-over-socket		
	
	char buffer[maxBuff];
	
	
	
	while (1) {
		// Read data into buffer.  We may not have enough to fill up buffer, so we
		// store how many bytes were actually read in bytes_read.
		cout << "hey" << endl;
		int bytes_read = read(fp, &buffer, sizeof(buffer));
		if (bytes_read == 0){			// We're done reading from the file
			close(fp);
			break;
		}

		if (bytes_read < 0) {
			// handle errors
	
			cout << "dude: " << errno <<endl;
		}

		// You need a loop for the write, because not all of the data may be written
		// in one call; write will return how many bytes were written. p keeps
		// track of where in the buffer we are, while we decrement bytes_read
		// to keep track of how many bytes are left to write.
		void *p = &buffer;
		while (bytes_read > 0) {
			int bytes_written = write(client, p, bytes_read);
			if (bytes_written <= 0) {
				// handle errors
			}
			bytes_read -= bytes_written;
			p += bytes_written;
		}
	}
}



