Client Server System with Shared Memory
For The Server:
Compile the program and run it in the command line with the syntax below.
•	Compiling Syntax: gcc server.c -o server -lpthread && ./server
The server will then start assuming all parameters are correct, and wait for a client to connect to the shared memory. Once requests from the client start coming in, the server will print out information in response to the behaviors of the client and progression of the factorization. 

For The Client:
In another terminal, compile the client program, and run it in the command line, following the syntax below.
•	Compiling Syntax: gcc client.c -o client -lpthread && ./client
NOTE: Ensure that the server is set up beforehand, with the same shared memory key. If this doesn’t happen, error checking is in place and the program will automatically terminate. 

Once connected, the user can then input one of 3 inputs:
•	Inputting ‘0’ will make the server start test mode and return to the client screen 3 sets of numbers from 0 to 99. 
•	Inputting any number greater than or equal to 1 will then return all the factors for the inputted number, and its rotations as denoted by the ROTATIONS_NUM (defined in shared_attrs.h – default is 32 as per the task sheet specifications).
•	Inputting ‘q’ will make the client and server terminate
 

This program was created on a linux system (Windows Subsystem for Linux) with compatibility for Cygwin64 Terminal.
Please ensure the program is run in such a linux system. 