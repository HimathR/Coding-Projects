// path, sys, time
#include <stdio.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include "include.h"

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

