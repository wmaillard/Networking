# client.py

import socket                   # Import socket module
import sys


#len(sys.argv) ->number of arguments
s = socket.socket()             # Create a socket object
host = socket.gethostname()     # Get local machine name
port = int(sys.argv[1])                    # Reserve a port for your service.

s.connect((host, port))
s.send("Hello server!")


					#Ideas for sending files came from here: http://www.bogotobogo.com/python/python_network_programming_server_client_file_transfer.php
with open('received_file', 'wb') as f:
    print 'file opened'
    while True:
        print('receiving data...')
        data = s.recv(1024)
        print('data=%s', (data))
        if not data:
            break
        # write data to a file
        f.write(data)

f.close()
print('Successfully get the file')
s.close()
print('connection closed')
