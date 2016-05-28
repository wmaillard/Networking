# ftclient.py
# CS 372-400
# Project 2: ftclient
# 5/28/16
# Description: This is the client side of a file transfer application that allows a client to request file listings and receive files. 
# More information about set-up, and command-line arguments can be found in the README.txt file.

import socket                 
import sys
import os.path


	
def main():

	
	theVars = initiateContact()
	theVars = makeRequest(theVars)
	receiveFile(theVars)

# Function: initialContact
# Arguments: none
# Description: Parses the commandline arguments and connects tries to connect to the given host.
# Returns: an object with the host, command, fileName, dataPort, and control connection number

def initiateContact():
	control = socket.socket()          
	formatErr = False
	# Parse out commandline arguments
	if len(sys.argv) == 6 or len(sys.argv) == 5:
		host = sys.argv[1]     
		port = int(sys.argv[2])
		command = (sys.argv[3])
		if len(sys.argv) == 5 :
			if(not str.isdigit(sys.argv[4])):
				formatErr = True
			else:
				dataPort = int(sys.argv[4])
				fileName = ""
		else :
			fileName = sys.argv[4]
			if(not str.isdigit(sys.argv[5])):
				formatErr = True
			else:
				dataPort = int(sys.argv[5])
	else:
		formatErr = True

	if formatErr:
		print "Please use the format 'ftclient <SERVER_HOST> <SERVER_PORT> <COMMAND> [FILENAME] <DATA_PORT>"
		control.close()
		os._exit(-1)

	#Make a connection
	control.connect((host, port))
	return {'host': host, 'command': command, 'fileName': fileName, 'dataPort': dataPort, 'control': control}

# Function: initialContact
# Arguments: theVars: An object with the host, command, fileName, dataPort, and control connection number
# Description: Sends a request to the server
# Returns: theVars with the data connection added if the connection was made successfully

def makeRequest(theVars):
	command = theVars['command']
	control = theVars['control']
	fileName = theVars['fileName']
	dataPort = theVars['dataPort']
	host = theVars['host']
	
	
	data = socket.socket()
	control.send(command + " " + fileName + " " + str(dataPort) + " " + socket.gethostname()) #Send the request
	response = control.recv(1024)
	responseSp = response.split()
	
	
	if responseSp[0] == "OK":						#If the server says everything is ok, connect on the dataPort.  Else close the control and exit.
		data.connect((host, dataPort))
		if(command == "-l"):
			print ("Receiving file listing from " + responseSp[1] + " on port: " + str(dataPort))
		elif(command == "-g"):
			print ("Receiving file '" + fileName + "' from " + responseSp[1] + " on port: " + str(dataPort))
	else:
		print "Error: " + response
		control.close()
		os._exit(-1)
		
	theVars['data'] = data
	return theVars

# Function: initialContact
# Arguments: theVars: An object with the host, command, fileName, dataPort, control connection number, and data connection number
# Description: Receives a file or the directory listing from the server, then closes the control connection
# Returns: none

def receiveFile(theVars):
	command = theVars['command']
	control = theVars['control']
	fileName = theVars['fileName']
	dataPort = theVars['dataPort']
	host = theVars['host']
	data = theVars['data']
	
	
	if command == "-l":					#Print the directory listing if that is what was requested
		theList = data.recv(4096)				
		print "These are the files:\n" + theList
	else:
					#Ideas for sending files came from here: http://www.bogotobogo.com/python/python_network_programming_server_client_file_transfer.php
		if(os.path.isfile(fileName)):
			newFileName = "new" + fileName
			while(os.path.isfile(newFileName)):
				newFileName = "new" + newFileName
			print("\n" + fileName + " already exists on your computer. \nI'll transfer in the file and call it " + newFileName + "\n")
			fileName = newFileName

		with open(fileName, 'wb') as newFile:
			filePieces = data.recv(1024)
			print 'receiving file...'
			while filePieces:				
				newFile.write(filePieces)
				filePieces = data.recv(1024)
		newFile.close()
		if True:			#change this to on success, maybe correct number of bytes
			print 'File successfully transferred\n'
		else:
			print 'Error transferring file\n'
	control.close() #close the control connection

if __name__ == '__main__':
	main()