#include "include.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void getfunc(const char *filename) // pass in a filename to get
{
    FILE *fptr; 
    char c;
    fptr = fopen(filename, "r"); // create file pointer and open file in read mode
    if (fptr == NULL)
    {
        printf("Error occurred when reading file! Please try again \n");
        return;
    }

    int i = 0;
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fptr) != -1) // read until the end of the file
    {
        printf("%s", line); 
        i++;
        if (i % 40 == 0) // at every 40 lines
        {
            char cont[5]; // await input before continuing until the next 40 lines or end of doc
            printf("\nPress Any Key To Continue...\n");
            scanf("%s", cont);
        }
    }
    printf("\n");
    // close file
    fclose(fptr);
}

