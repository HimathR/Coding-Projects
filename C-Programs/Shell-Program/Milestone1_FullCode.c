#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>

void sysfunc() 
{
    struct utsname info; // create a utsname structure (from the sys/utsname.h header)
    uname(&info); // call uname with a pointer to the struct, that will contain the info needed
    printf("The OS Name Is: %s\n", info.release);
    printf("The OS Version Is: %s \n", info.version); 
    printf("The CPU Type Is: %s With %d Cores \n", info.machine, get_nprocs()); // get CPU cores with get_nprocs from sys/sysinfo.h
}

void timefunc()
{
    time_t current_time; 
    struct tm *timeinfo; // create time structure
    time(&current_time); // get the current time
    timeinfo = localtime(&current_time); // use localtime function from time.h to convert current time to local time
    printf("The Current Local Time + Date Is: %s", asctime(timeinfo)); // shows the time in human readable format
}

void pathfunc()
{
    char currentpath[1000]; // initialise a char array to store path
    getcwd(currentpath, 1000); // use the getcwd function from unistd.h to get the path
    printf("The Current Working Directory Is: %s\n", currentpath); // print the part
}

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

int move(char *current_dir, char *destination_dir, char *name)
{
    // initialise struct from dirent to store files
    struct dirent *files;

    DIR *open_directory = NULL; // open directory ptr
    DIR *read_directory = NULL; // read directory ptr

    open_directory = opendir(current_dir);

    if (!open_directory)
        printf("Error with opening directory\n");
    else
    {
        while (files = readdir(open_directory))
        {
            if (strcmp(files->d_name, name) == 0) // if the current file in directory and file name passed in are the same
            {
                // copy over the file
                struct stat st_buf;
                stat(files->d_name, &st_buf); // check file exists in directory
                if (S_ISDIR(st_buf.st_mode))
                {
                    continue;
                }
                else if (S_ISREG(st_buf.st_mode))
                {
                    FILE *readfptr = fopen(files->d_name, "r"); // create file pointer to read filee
                    if (readfptr)
                    {
                        char destination_file[1000] = {0};
                        sprintf(destination_file, "%s/%s", destination_dir, files->d_name);

                        // create file pointer to write in file to destination
                        FILE *writefptr = fopen(destination_file, "w");
                        if (writefptr)
                        {
                            char buffer[1000] = {0}; /*Buffer to store files content*/
                            while (fgets(buffer, 1000, readfptr))
                            {
                                fputs(buffer, writefptr);
                            }
                            fclose(writefptr);
                        }

                        fclose(readfptr);
                    }
                    else
                    {
                        printf("\nFile Error");
                    }
                }
            }
        }

    }
    closedir(open_directory);
    return 0;
}

// you can't easily remove a non-empty directory without recursively removing all 
// contents inside the directory first
int remove_directory(const char *path)
{
    DIR *directory = opendir(path); // open the specific folder that should be removed using dir pointer
    size_t path_len = strlen(path);
    int r = -1;

    if (directory) // if directory successfully opened
    {
        struct dirent *p;
        // initialise struct from dirent to store files

        r = 0; // used to denote what the functions return (0 for effective completion, -1 otherwise generally)
        while (!r && (p = readdir(directory)))
        {
            int r2 = -1;
            char *buf;
            size_t len;

            // don't recurse on names with "." or ".."
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len); // allocate memory for the file

            if (buf)
            {
                struct stat statbuf;
                snprintf(buf, len, "%s/%s", path, p->d_name);  // get the name of the current file in directory
                if (!stat(buf, &statbuf)) 
                {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = remove_directory(buf); // recurse deeper into tree
                    else
                        r2 = unlink(buf); // unlink function deletes the file name in buf, will return 0 if successful
                }
                free(buf); // free memory allocated for buf 
            }
            r = r2; // set r2 back to 0 at end of recursion
        }
        closedir(directory);
    }

    if (!r) // once all files have been removed from directory, use rmdir to remove empty directory
        r = rmdir(path); // here, r will be 0 if successfully deleted directory

    return r;
}

void putfunc(char *cmd)
{
    int overwrite = 0;
    if (strstr(cmd, "-f")) // if the command has -f, it must be overwritten
    {
        overwrite = 1;
    }

    // get the number of files to account for
    int x = 0;
    int space = 0;
    while (x <= cmd[x])
    {
        if (cmd[x] == ' ')
        {
            space++;
        }
        x++;
    }

    char delim[] = " ";
    char *token = strtok(cmd, delim); // tokenise command
    int i = 0;
    char path[200];
    while (token != NULL)
    {
        token = strtok(NULL, delim);
        if (i == 0) // when i = 0, the token is the dirname
        {
            struct stat st = {0};
            if (stat(token, &st) == -1) // if it doesnt exist (check using stat function)
            {
                mkdir(token, 0700); // create directory
            }
            else
            {
                if (overwrite == 1) // if it already exists and overwrite is true
                {
                    printf("Directory Already Exists. Overwriting...\n");
                    getcwd(path, 200);
                    strcat(path, "/");
                    strcat(path, token);    // get the path, and concatenate dirname
                    remove_directory(path); // then remove the directory contents as it exists
                    mkdir(token, 0700);     // then remake the directory as an empty directory
                }
                else // if no overwrite (-f) variable given, print error
                {
                    printf("Error: Directory Already Exists\n");
                    break;
                }
            }
            // this is the new directory the file has to go to
            getcwd(path, 200);
            strcat(path, "/");
            strcat(path, token);
            printf("Sending File To This New Directory: %s\n", path);
            strcat(path, "/");
        }
        else if (i >= 1 && i <= space - 1 && strstr(token, "-f") == 0)
        {
            char pathcopy[100];
            strcpy(pathcopy, path); // copy original path which is the dest folder
            char temppath[100];
            getcwd(temppath, 200);
            // start moving files from current directory to pathcopy, which is the destination directory
            move(temppath, pathcopy, token);

            struct stat buffer;
            int exist = stat(token, &buffer);
            if (exist == 0) // see if the file moved proper with the stat function to make sure it moved
            {
                printf("%s has been moved\n", token);
            }
            else // if the file didn't exist or couldn't be moved, print error
            {
                printf("Error occurred with moving %s\n", token);
            }
        }
        else
        {
            break;
        }
        i++;
    }
}

int stack[100]; // global var stack
int top = 0;

int add(int a, int b) { return a + b; } // addition operation
int sub(int a, int b) { return b - a; } // subtraction operation 
void push(int value) { stack[top++] = value; } // puts a value on the top of the stack
int get_size() { return top; } // gets the current stack size (how many items still inside the stack)
int pop() // removes topmost element from stack
{
    if (top == 0) // if there's no operands
    {
        printf("Not enough operands in expression\n");
        exit(EXIT_FAILURE); 
    }
    return stack[--top]; // reduce stack size
}

double calcfunc(char *cmd)
{
    char *delimeter = " "; // delimeter is a space (equation is partioned by spaces)
    double result = -1; // variable to store result
    int token_length; // to account for double digit/triple digit nums
    char *token;

    token = strtok(cmd, delimeter); // tokenising using strtok
    while (token != NULL)
    {
        token_length = 0;
        while (token[token_length])
        {
            token_length++;
        }
        if (isdigit(token[0])) // if its an operand
        {
            push(atof(&token[0]));
        }
        else if (ispunct(token[0])) // if its an operator 
        {
            if (get_size() < 2) // check the expression is valid
            {
                printf("Error\n");
                return -1;
            }
            else
            {
                double operand1, operand2;
                operand1 = pop();
                operand2 = pop(); // pop two top most operands
                if (token[0] == '+') // if current token is a + operator
                {
                    // add the operands
                    result = add(operand1, operand2);
                }
                if (token[0] == '-') // same process but with subtraction
                {
                    result = sub(operand1, operand2);
                }
                push(result); // add that to the result
            }
        }
        token = strtok(NULL, delimeter);
    }
    // continue until only the final number in the stack is left, which is the result
    return pop();
}


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
