#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h> 
#include <netdb.h> 
#include <ctype.h>
#include <arpa/inet.h> // for inet_addr()
#define BUFFERLEN 100
#define SERVERIP "127.0.0.1"

// Compiling & Usage: gcc game_client.c -o game_client && ./game_client numbers HimathR 4444
// Syntax: int argc, char *argv[] / game_client <Game Type> <Server Name> <Port Number>
int main(int argc, char *argv[]) 
{
    if (argc < 3)
    {
        printf("Not Enough Arguments!\n");
        exit(1);
    }
    char port[5];
    strcpy(port, argv[3]);
    char msg[BUFFERLEN];

    struct sockaddr_in server;
    int client_sockfd;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVERIP);
    server.sin_port = htons(atoi(port)); 

    // Create The Socket
    if ((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Client Socket Creation Failed\n");
        exit(1);
    }
    // Connect To The Server
    if (connect(client_sockfd, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
        perror("Client Socket Connection Failed!\n");
        close(client_sockfd);
        exit(1);
    }

    printf("Connected\n");
    printf("-=+=- Starting The Numbers Game -=+=-\n");

    while (1)
    {
        recv(client_sockfd, msg, BUFFERLEN, 0); // Receive a message from the server

        if (strstr(msg, "TEXT")) // If the message contains the word "TEXT" or "ERROR"
        {
            char *tmp = strchr(msg, ' ');
            if (tmp != NULL)
            {
                tmp = tmp + 1; // remove the word "TEXT", then print message
            }
            printf("%s\n", tmp);
        }
        else if (strcmp(msg, "GO") == 0) // If the message contains the word "GO"
        {
            char input_string[BUFFERLEN], movestring[10];
            scanf("%s", input_string); // Accept an input
            if (strcmp(input_string, "quit") == 0) // Check if the input is to terminate
            {
                send(client_sockfd, "QUIT", BUFFERLEN, 0);
            }
            else // If it's not quit, check if the input is a number
            {
                for (int i = 0; input_string[i] != '\0'; i++)
                {
                    if (!isdigit(input_string[i])) // if the input has things other than numbers, instantly quit because it's a faulty msg
                        send(client_sockfd, "QUIT", BUFFERLEN, 0);
                    break;
                }
                strcpy(movestring, "MOVE ");
                strcat(movestring, input_string); // concatenate the "MOVE" string and number chosen and send to server 
                send(client_sockfd, movestring, BUFFERLEN, 0);
            }
        }
        else if (strcmp(msg, "END") == 0) 
        {
            printf("Quitting Game...\n");
            exit(1);
        }
    }

    return 0;
}
