# client.py

import socket                 
import sys


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
	data.connect(host, dataPort)
else:
	print "Error: " + response
	control.close()
	exit -1

if command == "-l":
	theList = control.recv(4096)				#is 4 MiB ok?
        print theList
else:
					#Ideas for sending files came from here: http://www.bogotobogo.com/python/python_network_programming_server_client_file_transfer.php
	with open(fileName, 'wb') as newFile:
		filePieces = control.recv(4096)
		while filePieces != 0:				#is 4 MiB ok?
			print 'receiving file...'
			# write data to a file
			newFile.write(filePieces)
			filePieces = control.recv(4096)
	newFile.close()
	if True:			#change this to on success, maybe correct number of bytes
		print 'File successfully transfered'
	else:
		print 'Error transferring file'
data.close()
control.close()

