#include "shared_attrs.h"

// GLOBALS
int test_mode = 0; 
Shared_Memory *shared_memory;
struct timeval last_update;
pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;

// CLASSES
struct Query
{
    unsigned long int num;
    unsigned long int *slots;
    int slot;
    char *server_flag;
    float *progress;
    struct timeval start_time;
} typedef Query;

// FUNCTIONS
int slot_available(unsigned long int *slots); // check if there are any slots open
void *progress_checker(void *arg); // tracks progress with threads for each slot
void print_prog(float progress); // prints the progress of each thread 
void finish_program(int handler); // used to remove shared memory and for quitting

// MAIN
// Compile with gcc client.c -o client -lpthread && ./client
int main(int argc, char **argv)
{
    gettimeofday(&last_update, NULL);
    signal(SIGINT, finish_program);
    key_t key = ftok("shmfile", SHARED_MEMORY_KEY);
    int shm_id = shmget(key, sizeof(Shared_Memory), 0666);
    if (shm_id < 0)
    {
        perror("ERROR: Shared Memory Unavailable - Has The Server Been Initialised?\n");
        exit(1);
    }
    shared_memory = (Shared_Memory *)shmat(shm_id, NULL, 0);
    /*if (shared_memory == -1)
    {
        perror("(Server) ERROR: Could Not Attach To Shared Memory\n");
        exit(1);
    }*/

    // init the queries
    Query queries[SLOT_NUM];
    for (int i = 0; i < SLOT_NUM; i++)
    {
        queries[i].server_flag = shared_memory->server_flag;
        queries[i].slots = shared_memory->slots;
        queries[i].slot = -1;
        queries[i].progress = shared_memory->progress;
    }
    printf("-=+=- Starting With %d Slots And %d Rotations -=+=-\n", SLOT_NUM, ROTATIONS_NUM);
    // create the threads
    pthread_t threads[SLOT_NUM];
    for (int i = 0; i < SLOT_NUM; i++)
    {
        pthread_create(&threads[i], NULL, &progress_checker, &queries[i]);
    }
    // detach the threads once they're finished
    for (int i = 0; i < SLOT_NUM; i++)
    {
        pthread_detach(threads[i]);
    }

    char cmd[100];
    unsigned long int number;
    while (1)
    {
        printf("Enter A Valid Command (0, q, or number): ");
        fgets(cmd, 100, stdin);
        if (cmd[0] == QUIT) // case one, user enters 'q' to quit
        {
            shared_memory->client_flag = QUIT;
            finish_program(1);
        }
        if (strlen(cmd) == 1 || !isdigit(cmd[0])) // case two, user just presses space OR enters an invalid letter
        {
            continue;
        }
        if (!slot_available(shared_memory->slots)) // if it passes top two checks, now check for available slots
        {
            printf("The server is full, please try later...\n");
            continue;
        }
        if (cmd[0] == '0') // if the command is 0
        {
            for (int i = 0; i < SLOT_NUM; i++) // check to see if theres any ongoing requests
            {
                if (shared_memory->slots[i] == -1)
                { 
                    test_mode = 1; 
                }
                else
                {
                    test_mode = 0; // if there is, set test mode to 0 and get out of test mode
                    break;
                }
            }
            if (test_mode == 0)
            {
                printf("ERROR: There's still ongoing requests...\n");
                continue;
            }
            else 
            {
                printf("Starting Test Mode...\n");
            }
        }
        else
        {
            test_mode = 0;
        }

        // If there's available slots, initialise handshake protocol
        shared_memory->number = strtoul(cmd, NULL, 0);
        number = shared_memory->number;
        // the client must set clientflag = 1 to indicate to
        // the server that new data is available for it to read
        // that is, if client_flag is "OCCUPIED" (1), server will read it
        shared_memory->client_flag = OCCUPIED;

        // this will make the program wait until the server can read in the client flag
        while (shared_memory->client_flag == OCCUPIED)
        {
        }

        // inform client of the slot being occupied
        printf("Slot %lu Will Be Used\n", shared_memory->number + 1);

        // updating the struct for the corresponding thread
        int slot = shared_memory->number;

        // reset the progress
        shared_memory->progress[slot] = 0.0f;
        // tell client new slot has been occupied
        queries[slot].slot = slot;
        // occupied number is:
        queries[slot].num = number;
        // the time when the request was accepted was:
        queries[slot].start_time;
        gettimeofday(&queries[slot].start_time, NULL);
        last_update.tv_sec = queries[slot].start_time.tv_sec;
    }

    return 0;
}

void print_prog(float progress)
{
    int pos = 10 * progress / 100;
    for (int i = 0; i < 10; ++i)
    {
        if (i < pos)
            printf("▓");
        else
            printf("_");
    }
    printf("] ");
}

void *progress_checker(void *arg)
{
    Query *query = (Query *)arg;
    struct timeval end_time, current;
    long double duration;
    int printed_progress;
    //printf("THREAD CREATED \n");

    while (1)
    {
        if (test_mode == 0) // if we aren't in test mode, give progress bar
        {
            // get current time,
            gettimeofday(&current, NULL);

            pthread_mutex_lock(&progress_mutex);
            if ((current.tv_sec - last_update.tv_sec) >= 0.5) // 500 Milliseconds
            {
                printed_progress = 0;
                for (int i = 0; i < SLOT_NUM; i++)
                {
                    if (query->server_flag[i] != FINISHED)
                    {
                        if (!printed_progress)
                        {
                            printf("Progress: ");
                            printed_progress = 1;
                        }
                        // + 1 because not 0 index
                        printf("Q%d:%.2f%% ", i + 1, query->progress[i]);
                        print_prog(query->progress[i]);
                    }
                }
                if (printed_progress)
                {
                    printf("\n");
                }
                last_update.tv_sec = current.tv_sec; // client has now updated again so reset the current time
            }
            pthread_mutex_unlock(&progress_mutex);
        }
        if (query->slot == -1)
        {
            continue;
        }
        if (query->server_flag[query->slot] == OCCUPIED)
        {
            if (test_mode == 1)
                printf("New Number: %lu \n", query->slots[query->slot]);
            else
                printf("New Factor For Query %d: %lu \n", query->slot + 1, query->slots[query->slot]);
            // data was received so set server flag to empty
            query->server_flag[query->slot] = EMPTY;

            // update our global var of the time of the last update
            gettimeofday(&last_update, NULL);
        }
        if (query->server_flag[query->slot] == FINISHED)
        {
            printf("QUERY COMPLETE FOR NUMBER %lu IN SLOT %d\n", query->num, query->slot + 1);
            gettimeofday(&end_time, NULL);
            // calculate time taken for thread to finish
            duration = ((end_time.tv_sec * 1000000 + end_time.tv_usec) - (query->start_time.tv_sec * 1000000 + query->start_time.tv_usec));
            printf("Time Taken: %.2Lf Seconds\n", duration / 1000000.0);
            query->slot = -1;
        }
    }
}

int slot_available(unsigned long int *slots)
{
    for (int i = 0; i < SLOT_NUM; i++) // go through the slots and find an empty one
    {
        if (slots[i] == -1)
        {
            return 1;
        }
    }
    return 0;
}

void finish_program(int handler)
{
    printf("\nFinishing Program...\n");
    shared_memory->client_flag = QUIT;
    printf("-=+=- Program Complete! -=+=-\n");
    exit(0);
}