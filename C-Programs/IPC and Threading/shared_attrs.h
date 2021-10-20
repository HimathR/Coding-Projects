// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // strcmp
#include <unistd.h>     // sleep()
#include <pthread.h>    // threading 
#include <sys/ipc.h>    // interprocess communication
#include <sys/shm.h>    // shared memory
#include <signal.h>     // for cleanup function
#include <time.h>       // random delays & progress tracking 
#include <sys/time.h>
#include <ctype.h>      // isalpha

// Basic Parameters
#define SHARED_MEMORY_KEY 8888
#define SLOT_NUM 4          // number of slots used by program (default 10)
#define ROTATIONS_NUM 24    // number of rotations to do (default 32)

// Handshaking Protocol Macros
#define OCCUPIED 1
#define EMPTY 0
#define QUIT 'q'
#define FINISHED 'f'

// Shared Memory Structure
struct Shared_Memory
{
    unsigned long int number;
    unsigned long int slots[SLOT_NUM];
    char  server_flag[SLOT_NUM];
    char  client_flag;
    float progress[SLOT_NUM];
} typedef Shared_Memory;

