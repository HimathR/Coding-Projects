
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

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
