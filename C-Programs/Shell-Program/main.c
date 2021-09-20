#include <stdio.h>
#include <string.h>
#include "include.h"

int main()
{
    char cmd[100];
    printf("Enter A Command: ");
    scanf("%[^'\n']s", cmd); // %[^'\n']s allows for inputs of full sentences

    while (strcmp(cmd, "quit") != 0) // continue while loop until quit command is run
    {
        // strstr checks to see if given string is present in the "cmd" input
        if (strstr(cmd, "calc ")) // TODO: just need to reverse the input string if needed
        {
            size_t ln = strlen(cmd) - 1;
            if (cmd[ln] == '\n')
                cmd[ln] = '\0';

            double result;
            result = calcfunc(cmd);
            if (result != -1)
            {
                printf("Result: %f\n", result);
            }
        }
        else if (strstr(cmd, "put")) 
        {
            putfunc(cmd);
        }
        else if (strcmp(cmd, "time") == 0)
        {
            timefunc();
        }
        else if (strcmp(cmd, "path") == 0)
        {
            pathfunc();
        }
        else if (strcmp(cmd, "sys") == 0)
        {
            sysfunc();
        }
        else if (strstr(cmd, "get"))
        {
            int i = 0;
            char *p = strtok(cmd, " ");
            char *array[2];

            while (p != NULL)
            {
                array[i++] = p;
                p = strtok(NULL, " ");
            }
            getfunc(array[1]);
        }
        else
        {
            printf("Invalid Input! Please Try Again...\n");
        }
        printf("Enter a command: ");
        getchar();
        scanf("%[^\n]s", cmd); // await another command
    }
    printf("-=+=- Program Finished! -=+=-\n");
    return 0;
}