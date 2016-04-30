William Maillard
CS 372-400
Project 1
5/1/16

This program was tested on flip2.engr.oregonstate.edu and flip3.engr.oregonstate.edu.

Use:
Compile both the chatclient and chatserve by just typing make.

Run chatserve and chatclient with the following arguments:
chatserve <port number>
chatclient <host name> <port number>

Start up chatserve first.  Chatserve will then wait for a connection on the
port specified.  Next, start chatclient.  Chatclient will ask for your name
and an initial message to send to chatserve.  Once the first message is recieved,
a connection has been made and chat can begin.

Note: Be sure to alternate between chatserve and chatclient when chatting.

To terminate the chat, type "\quit" without quotes on either the server or the client.
Doing so will end the process for the client and the server will stay active and listen 
for another client on the initial port that was specified.  Hit ctrl-c to end clientserve.
