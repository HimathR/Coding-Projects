#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <ctype.h>
#include <signal.h> 
#define BUFFERLEN 100

// Functions
int create_connection(int sockfd);
void numbers_game(int *players_fd, int num_players);
int quitprompt(int *players_fd, int current_player, int num_players, int originalcount);
void handler(int signo) { return; }

// Compiling & Usage: gcc game_server.c -o game_server && ./game_server 4444 numbers 2
// Syntax: int argc, char *argv[], game_server <Port Number> <Game Type> <Game arguments>
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Not Enough Arguments!\n");
        exit(1);
    }

    int num_players = atoi(argv[3]); // convert from string to integer
    char port[5];
    strcpy(port, argv[1]);          // strcpy(port, "4444");
    int player_sockfd[num_players]; // store player socket descriptors in an array
    int server_sockfd;

    if (num_players < 2 || num_players > 4) // Make sure player number is valid
    {
        printf("Invalid Player Number! Try Again...\n");
        exit(1);
    }

    // Create a TCP Socket
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        close(server_sockfd);
        perror("Server Socket Creation Failure\n"); // Error Checking
        exit(1);
    }
    // Prepare the Socket Address
    struct sockaddr_in serveradd, clientadd;
    serveradd.sin_family = AF_INET;
    serveradd.sin_addr.s_addr = INADDR_ANY;
    serveradd.sin_port = htons(atoi(port));
    // Bind Address To Socket
    if (bind(server_sockfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) == -1)
    {
        close(server_sockfd);
        perror("Server Socket Bind Failed\n"); // Error Checking
        exit(1);
    }
    if (listen(server_sockfd, num_players) == -1) // Error Checking
    {
        close(server_sockfd);
        perror("Listen Failed");
        exit(1);
    }
    printf("\nStarting The Game. We Need %d Players \n", num_players);
    for (int i = 0; i < num_players; i++)
    {
        player_sockfd[i] = create_connection(server_sockfd); // get the player sockfd
    }
    close(server_sockfd);

    // Start Game Loop
    numbers_game(&player_sockfd[0], num_players);

    return 0;
}

int create_connection(int sockfd)
{
    struct sockaddr_storage client_addr; // The connecting client's address info
    socklen_t client_len = sizeof(client_addr);
    int new_fd; // Socket descriptor for incoming connection

    // Accept new connection
    new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (new_fd == -1)
    {
        perror("Accept");
    }
    printf("New Connection Has Been Accepted!\n");
    // Tell client the game is starting
    send(new_fd, "TEXT Welcome To The Game!", BUFFERLEN, 0);
    return new_fd;
}

int quitprompt(int *players_fd, int current_player, int num_players, int originalcount)
{
    send(players_fd[current_player], "END", BUFFERLEN, 0); // send to faulty client
    close(players_fd[current_player]);
    players_fd[current_player] = 0; // set faulty client in array to 0
    num_players--;       // reduce number of players
    if (num_players <= 1) // if there aren't enough players, terminate
    {
        printf("There aren't enough players to continue! Terminating...\n");
        for (int j = 0; j < originalcount + 1; j++) // send END message to all remaining clients
        {
            if (players_fd[j] != 0) // make sure not to interact with faulty clients
            {
                send(players_fd[j], "TEXT There aren't enough players, You win", BUFFERLEN, 0);
                send(players_fd[j], "END", BUFFERLEN, 0);
                close(players_fd[j]);
            }
        }
        exit(1);
    }
    return num_players;
}

void numbers_game(int *players_fd, int num_players)
{
    printf("Starting Numbers Game\n");

    int total = 0;       // storing current score
    int new_number = 0;  // storing incoming input by a client
    char msg[BUFFERLEN]; // storing client messages
    int originalcount = num_players; // for iterations as num_players will change as players leave
    while (1)
    {
        printf("\n");
        for (int i = 0; i < originalcount; i++)
        {
            if (players_fd[i] == 0) // this is a disconnected client
            {
                continue; // skip the iteration for this client as they no longer exist
            } 
            char buffer[10];
            char textstring[50];
            strcpy(textstring, "TEXT The Current Sum Is: ");
            snprintf(buffer, 10, "%d", total);
            strcat(textstring, buffer);
            send(players_fd[i], textstring, BUFFERLEN, 0);
            send(players_fd[i], "TEXT It’s your turn now! Enter An Input: ", BUFFERLEN, 0);
            send(players_fd[i], "GO", BUFFERLEN, 0);


            // Start Timeout Alarm 
            struct sigaction timeouttracker;
            timeouttracker.sa_handler = handler;
            sigemptyset(&timeouttracker.sa_mask);
            timeouttracker.sa_flags = 0;
            sigaction(SIGALRM, &timeouttracker, NULL);
            alarm(10); // set alarm time here
            if (recv(players_fd[i], msg, BUFFERLEN, 0) != -1)
            {
                alarm(0); // cancel the alarm
            }
            else
            {
                printf("PLAYER %d TIME OUT\n", i+1);
                send(players_fd[i], "TEXT ERROR You were too slow! Your Connection Was Already Terminated. Sorry! ", BUFFERLEN, 0);
                quitprompt(players_fd, i, num_players, originalcount);
                continue; // skip to next iteration
            }

            if (strstr(msg, "MOVE")) // if client replies with a move command
            {
                char *tmp = strchr(msg, ' ');
                if (tmp != NULL)
                {
                    tmp = tmp + 1;
                }
                new_number = atoi(tmp); // remove the "MOVE" string and extract number

                int errors = 0;
                // make sure new number is valid
                while (new_number <= 0 || new_number >= 10)
                {
                    errors++;
                    if (errors >= 5) // if an incorrect number is given 5 times, terminate
                    {
                        num_players = quitprompt(players_fd, i, num_players, originalcount);
                        continue;
                    }

                    send(players_fd[i], "TEXT ERROR Bad Input - Number must be between 1 and 9! Try Again: ", BUFFERLEN, 0);
                    send(players_fd[i], "GO", BUFFERLEN, 0);
                    recv(players_fd[i], msg, BUFFERLEN, 0);

                    char *tmp = strchr(msg, ' ');
                    if (tmp != NULL)
                    {
                        tmp = tmp + 1;
                    }
                    new_number = atoi(tmp);
                }

                total += new_number;                                                    // add valid new number to current total
                printf("Player %d Chose %d. Total is %d.\n", i + 1, new_number, total); // print additional info to server

                if (total >= 30) // if the win condition (total >= 30) is met:
                {
                    printf("Player %d won the game\n", i + 1);
                    for (int j = 0; j < originalcount; j++) // loop through each player
                    {
                        if (i == j) // for the current player, send a "you won" message and terminate
                        {
                            send(players_fd[j], "TEXT You Won!", BUFFERLEN, 0);
                            send(players_fd[j], "END", BUFFERLEN, 0);
                        }
                        else // for all other players, send a "you lost" message and terminate
                        {
                            send(players_fd[j], "TEXT You Lost!", BUFFERLEN, 0);
                            send(players_fd[j], "END", BUFFERLEN, 0);
                        }
                    }
                    return;
                }
                else if (total < 30) // if the total is less than 30, continue to next round
                {
                    send(players_fd[i], "TEXT Number Recorded!", BUFFERLEN, 0);
                }
            }
            else // for QUIT AND Invalid protocol, terminate
            {
                num_players = quitprompt(players_fd, i, num_players, originalcount);
            }
        }
    }
}
