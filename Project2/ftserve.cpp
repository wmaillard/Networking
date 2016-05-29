/*William Maillard
CS 372-400
Project 2: ftserve
5/28/16
Description: This is the server side of a file transfer application that allows a client to request file listings and receive files.  More information about set-up, and command-line
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
#include <dirent.h>
#include <fstream>


using namespace std;
int startUp(const char *port);
int handleRequest(int &control, int &data);
int acceptConnection(int server);
int receiveMessage(int client, string *received);
int sendMessage(string message, int connection);
int sendMyFile(string file, int connection);
bool fexists (string &file); 

int main(int argc, char *argv[])								//argv[1] is the port number
{
    int errno;
	int data, control;
	//Check number of command line arguments
	if(argc != 2){
		printf("Please follow this format: ftserver <port number>\n");
		return -1;
	}
	const char* controlPort = argv[1];															

	printf("Connecting on port %s...\n", controlPort);
	control = startUp(controlPort);									//Start listening on port
	if(control == -1) return -1;
	
	while(true){												//Keep listening open even after file transfer is done (until SIGINT)
		printf("\nWaiting for a request from the client on port, %s...\n", controlPort);
		
		if(handleRequest(control, data) == 0){
			cout << "\nSuccessfully transferred data over the data connection to the client" << endl;
		}
		else{
			cout << "\nThere was an error while communicating with the client" << endl;
		}

	} //end main while
	

    return 0;
}
//A lot of the info for these functions comes from Beej's guide										//http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html


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

//Function: handleRequest
//Arguments: The control and the data connections
//Parses the clients request from the control connection and sends an error if needed.
//If no error, opens a data connection and sends either the file listing or the file requested
//Prints messages and returns -1 if there is an error

int handleRequest(int &control, int &data){
		
	int controlConn, dataConn;
	bool formatError = false;
	bool dataConnectionErr = false;
	bool anyError = false;
	string dataPort = "";
	
	controlConn = acceptConnection(control);
	if(controlConn == -1) return -1;

	vector<string> arguments;
	string received = "";
	string error;


	if(receiveMessage(controlConn, &received) == -1){	//Receive first message 
		//handle error
		anyError = true;
	}
	//Parse out request from client
	for(int i = 0; i < received.length(); i++){
		while(i < received.length() && received.at(i) == ' '){
			i++;
		}
		string newArg = "";
		while(i < received.length() && received.at(i) != ' ' ){
			newArg += received.at(i);
			i++;
		}
		arguments.push_back(newArg);
	}

	dataPort = arguments[arguments.size() - 2];
	
	data = startUp(dataPort.c_str());  //Start up data connection
	
	//Check the for errors connecting, argument errors, and formatting errors
	if(arguments[0] != "-l" && arguments[0] != "-g"){
		error = "Your command, " + arguments[0] + " is not recognized";
		formatError = true;
	}
	else if(arguments.size() == 4 && arguments[0] == "-l"){
		error = "Your command '-l' doesn't match the correct number of arguments\n Please use the format 'ftclient <SERVER_HOST> <SERVER_PORT> <COMMAND> [FILENAME] <DATA_PORT>";
		formatError = true;
	}
	else if(data == -1){
		cout << "Tried to send data to " << arguments[arguments.size() - 1] << " on port " << arguments[arguments.size() - 2] << " but could not connect." << endl;
		error = "Could not connect on port " + arguments[arguments.size() - 2];
		formatError = true;
	}

	else if(arguments.size() == 4 && !fexists(arguments[1])){
		error = "File, '" + arguments[1] + "', does not exist";
		cout << "File '" << arguments[1] << "'requested on port " << arguments[arguments.size() - 2] << endl;
		cout << error << endl;
		formatError = true;
	}
	//If there was a format error then send an error messsage over the control connection
	if(formatError){
		sendMessage(error, controlConn); 
		formatError = false;
		error = "";
		anyError = true;

	}
	//If there aren't any errors then send 'OK' and then send the appropriate data
	else if(!anyError){
		char hostname[128];
		gethostname(hostname, sizeof hostname);
		
		string message = "OK ";
		message += hostname;
		sendMessage(message, controlConn);
		dataConn = acceptConnection(data);
		int sent;
		
		
		//Send the requested file
		if(arguments.size() == 4){
			  cout << "\nConnection from " << arguments[3] << endl;
			  cout << "File '" << arguments[1] << "' requested on port " << arguments[arguments.size() - 2] << endl;
			  cout << "Sending '" << arguments[1] << "' to " << arguments[3] << ":" << arguments[arguments.size() - 2] << endl;
			  sent = sendMyFile(arguments[1], dataConn);
		}
		//Send the directory listing
		else{  //Directory listing ideas taken from here: http://www.gnu.org/software/libc/manual/html_node/Simple-Directory-Lister.html
			cout << "\nConnection from " << arguments[2] << endl;
			cout << "List directory requested on port " << arguments[arguments.size() - 2] << endl;
			cout << "Sending directory contents to " << arguments[2] << ":" << arguments[arguments.size() - 2] << endl;
				
			DIR *dp;
			struct dirent *ep;
			string dirList = "";
			
			dp = opendir ("./"); //Open the working directory
			if (dp != NULL){
				while (ep = readdir (dp)){  //Concactenate the directory info onto a string
					dirList += ep->d_name;
					dirList += "\n";
			}
			closedir (dp);
			}
			else{
				perror ("Couldn't open the directory");
			}
			sent = sendMessage(dirList, dataConn);  //Send the directory listing
		}

		if(sent == 1){
			anyError = false;
			return 0;
		}
		else if(sent == -1){
			sendMessage("Error sending file", controlConn);
			return -1;
		}
	}

	//If there was not an error with the data connection, shut it down
	if(!anyError){
		if(shutdown(dataConn, 2) != 0){							//Shutdown connection
			printf("Error shutting down data connection: %d", errno);
			anyError = true;
		}
	}
	if(dataConnectionErr || formatError || anyError){
		return -1;
	}
	else return 0;
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
	
	int maxBuff = 1024;					//The idea of how to convert a buffer into a string came from here: http://stackoverflow.com/questions/18670807/sending-and-receiving-stdstring-over-socket		
	
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

//Function: sendMessage
//Arguments: message and connection number
//Send a message
//Returns -1 if there was a problem and 0 on success.

int sendMessage(string message, int connection){
			

		if( send(connection, message.c_str(), message.size(), 0) == -1){
			cout << "Error sending message: " << errno << endl;
			return -1;
		}
	
	return 0;
}

//Function: sendMyFile
//Arguments: file and connection number
//Send a message
//Returns -1 if there was a problem and 0 on success.

int sendMyFile(string fileName, int connection){  
	
	int fp = open(fileName.c_str(), O_RDONLY); //Open the file for reading
	int maxBuff = 1024;						
	char buffer[maxBuff];
									//The idea in this while loop was borrowed from here: http://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
	while (true) {  
		int dataIn = read(fp, &buffer, sizeof(buffer));
		if (dataIn == 0){
			close(fp);
			return 0;
		}
		if (dataIn < 0) {
			cout << "Error reading file to send: " << errno << endl;
			return -1;
		}
		void *buffPointer = &buffer;
		while (dataIn > 0) {
			int dataOut = write(connection, buffPointer, dataIn);
			if (dataOut <= 0) {
				cout << "Error writing file to socket stream: " << errno << endl;
			}
			dataIn -= dataOut;
			buffPointer += dataOut;
		}
	}
}
//Function: fexists
//Arguments: file name string
//check if a file exists
//true if it exists, false if not

bool fexists (string &file) { //Borrowed from here, just tests if a file exists: http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
    ifstream fp(file.c_str());
    return fp.good();
}


