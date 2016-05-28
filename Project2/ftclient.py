# client.py

import socket                 
import sys
import time


#len(sys.argv) ->number of arguments
control = socket.socket()          
data = socket.socket()

if len(sys.argv) == 6 or len(sys.argv) == 5:
	host = sys.argv[1]     
	port = int(sys.argv[2])
	command = (sys.argv[3])
	if len(sys.argv) == 5 :
		dataPort = int(sys.argv[4])
		fileName = ""
	else :
		fileName = sys.argv[4]
		dataPort = int(sys.argv[5])
else:
	print "Please use the format 'ftclient <SERVER_HOST> <SERVER_PORT> <COMMAND> [FILENAME] <DATA_PORT>"
	exit -1


control.connect((host, port))
control.send(command + " " + fileName + " " + str(dataPort)) 
response = control.recv(1024)

if response == "OK":
	time.sleep(1)
	data.connect((host, dataPort))
	print ("Connecting to data host: " + host + " on port: " + str(dataPort))
else:
	print "Error: " + response
	control.close()
	exit -1

if command == "-l":
	theList = data.recv(4096)				#is 4 MiB ok?
        print "These are the files:\n" + theList
else:
					#Ideas for sending files came from here: http://www.bogotobogo.com/python/python_network_programming_server_client_file_transfer.php
	with open("new" + fileName, 'wb') as newFile:
		while True:				#is 4 MiB ok?
			print 'receiving file...'
			filePieces = data.recv(4096)
			# write data to a file
			print 'pieces: ' + filePieces
			if not filePieces:
				break
			newFile.write(filePieces)
	newFile.close()
	if True:			#change this to on success, maybe correct number of bytes
		print 'File successfully transfered'
	else:
		print 'Error transferring file'
data.close()
control.close()

