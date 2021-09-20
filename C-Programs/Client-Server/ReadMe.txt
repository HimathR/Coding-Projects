Client Server System - Numbers Game

Instructions For The Server:
Compile the program and run it in the command line:
	Compiling Syntax: gcc game_server.c -o game_server
	Arguments: Game_server <port number> <game type> <player number> 
	Execution Syntax Example: ./game_server 4444 numbers 2
The server will then start (assuming all parameters are correct)

Instructions For The Client:
In a different device or another terminal, compile the client program, and run it in the command line 
	Compiling Syntax: gcc game_client.c -o game_client
	Arguments: game_client <game type> <server name> <port number>
	Execution Syntax Example: game_client numbers mypc 4444
NOTE: Ensure that the server is set up beforehand, and that the port number specified is the same as 
the one used by the server. Once connected, the user must wait for the specified number of players needed 
for the game to begin to join. Once the sufficient amount of players is connected, the server will prompt the 
user when it is their turn. The user can then input “quit” to leave the game or a number between 1 and 9. 
A lack of input for more than 30 seconds will result in the termination of the current client.
If the current client, or another client reaches the goal total score greater than 30 first, 
or there are no longer enough players to continue the game, the client will be disconnected, ending the program. 

This program was created on a linux system (Windows Subsystem for Linux) with compatibility for Cygwin64 Terminal.
Please ensure the program is run in such a linux system. 