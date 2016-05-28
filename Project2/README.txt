William Maillard
CS 372-400
Project 2
5/28/16

This program was tested on flip3.engr.oregonstate.edu with flip3.engr.oregonstate.edu.

*Extra Credit: This program can transfer any type of file

Use:
Compile ftserve by typing 'make'
ftclient.py is a python file, details on how to run it are below.

Run chatserve and chatclient with the following arguments,
arguments in [brackets] are optional:

ftserve <SERVER_PORT>
python ftclient.py <SERVER_HOST> <SERVER_PORT> <COMMAND> [FILENAME] <DATA_PORT>

COMMAND options:
'-l' lists the file in the servers working directory
'-g' gets the FILENAME specified from the server and transfers it to the client

Start up ftserve first.  Ftserve will then wait for a control connection on the
port specified.  Next, start ftclient.py.  If all of the command line arguments are correct,
a data connection will be created on DATA_PORT and either the list of files in the 
server's working directory will be sent to the client or the requested FILENAME will be sent to 
the client.  
If there is an error before establishing a data connection, the server will send out the error
on the control connection and the client will print it.
If the file requested by the client has the same name as a file in the clients working directory,
the requested file will be copied in with a new name and a message about this will be sent to
the client on the control connection and printed.


Note: Be sure to alternate between chatserve and chatclient when chatting.

ftclient will terminate after every request
ftserve will remain running until SIGINT (ctrl-c) to end
