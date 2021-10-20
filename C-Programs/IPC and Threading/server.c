#include "shared_attrs.h"

// GLOBALS
int shm_id;
int runcount = 0;

// Data Structures
struct Job
{
    int slot;              // Slot Number Corresponding To Thread
    unsigned long int num; // The number rotated itself
} typedef Job;

struct Job_Queue
{
    Job *jobs;
    pthread_mutex_t pop_mutex;
    pthread_mutex_t add_mutex;
    int head;
    int tail;
    int size;
    int length;
} typedef Job_Queue;

struct Thread_Pool
{
    unsigned int capacity;
    unsigned int used;
    pthread_t *pool;
    Job_Queue *job_queue;

} typedef Thread_Pool;

struct Thread_Args
{
    Shared_Memory *shared_memory;
    pthread_mutex_t *slot_mutex;
    Job_Queue *job_queue;
    char **progress;
    int *remaining_jobs;
} typedef Thread_Args;

// FUNCTIONS
// For getting the rotated numbers
void right_rotate(unsigned long int **rotations, int index, unsigned long int number); // done
void allocate_rotations(unsigned long int ***rotations);

// For doing each job
void create_queue(Job_Queue *queue, int length);
Job take_job(Job_Queue *job_queue);
int add_new_job(Job_Queue *job_queue, Job val);

// For each thread factorisation
void create_thread_pool(Thread_Pool *tp, int capacity, Job_Queue *job_queue, Thread_Args *thread_args, void *function);
void *thread_factorise(void *args); // for the factorisation thread activities

// For test mode:
void *test_function(void *args);                                                    // for the test mode activities
float RandomFloat(float min, float max);                                            // helper function for random time delay
void job_rotate(unsigned long int **rotations, int slot, unsigned long int number); // for test function thread variants

// for finishing up
void finish_program(int handler);

// MAIN
// Compile with gcc server.c -o server -lpthread && ./server
int main(int argc, char **argv)
{
    signal(SIGINT, finish_program);

    // shared memory
    Shared_Memory *shared_memory; // create shared memory struct
    key_t key = ftok("shmfile", SHARED_MEMORY_KEY);
    shm_id = shmget(key, sizeof(Shared_Memory), IPC_CREAT | 0666);
    if (shm_id < 0)
    {
        perror("(Server) ERROR: Shared Memory Couldn't Be Connected To\n");
        exit(1);
    }
    shared_memory = (Shared_Memory *)shmat(shm_id, NULL, 0); //  update the pointer to SM
    /*if (shared_memory == -1)
     {
          perror("(Server) ERROR: Could Not Attach To Shared Memory\n");
          exit(1);
     }*/

    // Initialise shared memory
    shared_memory->client_flag = 0;
    shared_memory->number = 0;

    for (int i = 0; i < SLOT_NUM; i++)
    {
        shared_memory->slots[i] = -1;             // means empty
        shared_memory->server_flag[i] = FINISHED; // finished
        shared_memory->progress[i] = 0;
    }
    printf("Initialisation Of Shared Memory Completed!\n");

    // rotations
    unsigned long int **rotations;
    allocate_rotations(&rotations);

    // slot mutex
    pthread_mutex_t slot_mutex[SLOT_NUM];
    for (int i = 0; i < SLOT_NUM; i++)
    {
        pthread_mutex_init(&slot_mutex[i], NULL);
    }

    // how many jobs remaining per job - zeroing too
    int *remaining_jobs = (int *)calloc(sizeof(int), sizeof(int) * SLOT_NUM);

    // setup threadpool and jobqueue
    Thread_Pool thread_pool;
    Job_Queue job_queue;
    Job job_info;

    // setup thread args
    Thread_Args thread_args;
    thread_args.shared_memory = shared_memory;
    thread_args.slot_mutex = slot_mutex;
    thread_args.job_queue = &job_queue;
    thread_args.remaining_jobs = remaining_jobs;

    create_queue(&job_queue, 320); // set up the queue with enough memory for 320 jobs

    int slot_to_use;
    int first_run = 0;
    while (1)
    {
        // the client quits the server
        if (shared_memory->client_flag == QUIT)
        {
            finish_program(0);
        }
        if (shared_memory->client_flag == OCCUPIED)
        {
            printf("Number Entered Is: %lu \n", shared_memory->number);
            if (shared_memory->number == 0) // START TEST MODE
            {
                create_thread_pool(&thread_pool, 30, &job_queue, &thread_args, test_function);
                // figure out what slot is usable
                slot_to_use = 1;

                // do the rotations and add all the rotations to the job queue
                job_rotate(rotations, slot_to_use, shared_memory->number);

                // number of jobs to do - 10 per thread
                remaining_jobs[slot_to_use] = 10;

                int client_num = 3;
                for (int t = 0; t < client_num; t++)
                {
                    pthread_mutex_lock(&job_queue.add_mutex);
                    for (int i = 0; i < 10; i++) // add all the jobs for the current slot
                    {
                        job_info.slot = slot_to_use; // the current free slot
                        job_info.num = rotations[slot_to_use][i];
                        add_new_job(&job_queue, job_info); // add new job into job queue
                    }
                    pthread_mutex_unlock(&job_queue.add_mutex);
                }
                // slot - give intial value of 0
                shared_memory->slots[slot_to_use] = EMPTY;
                shared_memory->server_flag[slot_to_use] = EMPTY;
                shared_memory->number = slot_to_use; // Tells the client straight away which slot is being occupied
                shared_memory->client_flag = EMPTY;  // client flag = 0 means request has been accepted*/
            }
            else // START FACTORISATION
            {
                // figure out what slot is usable
                first_run++;
                if (first_run == 1)
                {
                    create_thread_pool(&thread_pool, SLOT_NUM * ROTATIONS_NUM, &job_queue, &thread_args, thread_factorise);
                }
                slot_to_use = -1;
                for (int i = 0; i < SLOT_NUM; i++)
                {
                    if (shared_memory->slots[i] == -1)
                    {
                        slot_to_use = i;
                        break;
                    }
                }
                if (slot_to_use != -1)
                {
                    printf("There is an available slot and its index is: %d \n\n", slot_to_use);
                }

                // do the rotations and add all the rotations to the job queue
                right_rotate(rotations, slot_to_use, shared_memory->number);

                // number of jobs to do = number of rotations (default of 32)
                remaining_jobs[slot_to_use] = ROTATIONS_NUM;

                pthread_mutex_lock(&job_queue.add_mutex);
                for (int i = 1; i <= ROTATIONS_NUM; i++) // add all the jobs for the current slot
                {
                    job_info.slot = slot_to_use;              // the current free slot
                    job_info.num = rotations[slot_to_use][i]; // 32 indexes of i corresponding to rotated numbers
                    //printf("ADD %lu\n", job_info.num);
                    add_new_job(&job_queue, job_info); // add new job into job queue
                }
                pthread_mutex_unlock(&job_queue.add_mutex);

                // slot - give intial value of 0
                shared_memory->slots[slot_to_use] = EMPTY;
                shared_memory->server_flag[slot_to_use] = EMPTY;
                shared_memory->number = slot_to_use; // Tells the client straight away which slot is being occupied
                shared_memory->client_flag = EMPTY;  // client flag = 0 means request has been accepted
            }
        }
    }
    return 0;
}

void right_rotate(unsigned long int **rotations, int slot, unsigned long int number)
{
    unsigned long int rotated_number;
    for (int i = 1; i < ROTATIONS_NUM + 1; i++) // rotated given number of times (default 32)
    {
        rotated_number = (number >> i) | (number << ROTATIONS_NUM - i);
        //printf("Rotated %d Times: %lu\n", i, rotated_number); // for debugging
        rotations[slot][i] = rotated_number; // add to rotations 2d array allocated for earlier
    }
}

void allocate_rotations(unsigned long int ***rotations)
{
    (*rotations) = malloc(sizeof(unsigned long int *) * SLOT_NUM); // Allocate enough memory for all 10 slots
    if (rotations == NULL)
    {
        perror("ERROR: Malloc Failed\n");
        exit(1);
    }
    for (int i = 0; i < SLOT_NUM; i++)
    {
        // As it's a 2D Array, allocate memory for enough rotation number variations too
        (*rotations)[i] = malloc(sizeof(unsigned long int) * ROTATIONS_NUM);
        if ((*rotations)[i] == NULL)
        {
            perror("ERROR: Malloc Failed\n");
            exit(1);
        }
    }
}

void *thread_factorise(void *args)
{
    // printf("Starting New Thread\n");
    Thread_Args *thread_args = (Thread_Args *)args;
    Job_Queue *job_queue = thread_args->job_queue;
    Job to_do;
    int prog;

    while (1)
    {
        to_do.slot = -1;
        pthread_mutex_lock(&job_queue->pop_mutex);
        if (job_queue->size >= 1) // MUTEX LOCK
        {
            to_do = take_job(job_queue); // critical
        }
        pthread_mutex_unlock(&job_queue->pop_mutex); // MUTEX UNLOCK
        if (to_do.slot == -1 || to_do.num == 0)      // make sure that there are still jobs to be done
        {
            continue;
        }
        //printf("Processing %lu\n", to_do.num);
        for (unsigned long int i = 2; i <= to_do.num; i++) // commence trial division
        {
            if (to_do.num % i == 0)
            {
                int isPrime = 1;
                for (unsigned long j = 2; j <= i / 2; j++)
                {
                    if (i % j == 0)
                    {
                        isPrime = 0;
                        break;
                    }
                }
                if (isPrime == 1) // if the number found is prime, update the client with that number
                {
                    // update the slot with the factor only once client has read it
                    while (thread_args->shared_memory->server_flag[to_do.slot] == OCCUPIED)
                    {
                    }
                    //printf("FACTOR for slot %d: %lu \n", to_do.slot + 1, i);
                    sleep(0.1); // delay f
                    // mutex for the slot index
                    pthread_mutex_lock(&thread_args->slot_mutex[to_do.slot]);
                    // update the value in the slot with the factor
                    thread_args->shared_memory->slots[to_do.slot] = i;
                    pthread_mutex_unlock(&thread_args->slot_mutex[to_do.slot]);
                    // tell the client there is a new factor
                    thread_args->shared_memory->server_flag[to_do.slot] = 1;
                }
            }
        }
        thread_args->remaining_jobs[to_do.slot]--;                      // one less job to do in the slot
        float jobs_remaining = thread_args->remaining_jobs[to_do.slot]; // needed for progress calculation
        int tempprog = ((ROTATIONS_NUM - jobs_remaining) / ROTATIONS_NUM) * 100;
        prog = ((tempprog + 5 / 2) / 5) * 5; // this will make sure progress is in increments of 5%
        /*if (prog <= 100) // Reporting on server
        {
            printf("Slot %d is %d%% Finished With %d Jobs Left  \n", to_do.slot + 1, prog, thread_args->remaining_jobs[to_do.slot]);
        }*/
        thread_args->shared_memory->progress[to_do.slot] = prog;
        // >= 95 because of rounding to nearest 5% can cause program to hang otherwise
        if (thread_args->remaining_jobs[to_do.slot] <= 0 || prog >= 95) 
        {
            // signify that this slot is finished
            thread_args->shared_memory->server_flag[to_do.slot] = FINISHED;
            thread_args->shared_memory->slots[to_do.slot] = -1;
            printf("Slot %d Is Finished \n", to_do.slot + 1);
        }
    }
}

void create_thread_pool(Thread_Pool *tp, int capacity, Job_Queue *job_queue, Thread_Args *thread_args, void *function)
{
    (*tp).capacity = capacity;
    (*tp).used = 0;
    (*tp).pool = (pthread_t *)malloc(sizeof(pthread_t) * capacity);

    // create the threads
    for (int i = 0; i < (*tp).capacity; i++)
    {
        if (pthread_create(&(*tp).pool[i], NULL, function, (void *)thread_args))
        {
            perror("ERROR: Thread Creation Failed\n");
            exit(1);
        }
    }
    for (int i = 0; i < (*tp).capacity; i++)
    {
        pthread_detach((*tp).pool[i]);
    }
    printf("Created threads\n");
}

int add_new_job(Job_Queue *job_queue, Job val)
{
    if ((*job_queue).size < (*job_queue).length) // check that the max length hasn't been exceeded
    {
        // add the new job
        (*job_queue).tail = ((*job_queue).tail + 1) % (*job_queue).length;
        (*job_queue).jobs[(*job_queue).tail] = val;
        (*job_queue).size++;
        return 1;
    }
    return 0;
}

Job take_job(Job_Queue *job_queue)
{
    if ((*job_queue).size == 0) // if theres no jobs left
    {
        Job a = {-1, -1};
        // by setting this as -1, -1, in functions its an additional check
        // to make sure that the thread function knows there are no jobs left
        return a;
    }
    // remove the latest job:
    Job pop;
    pop = (*job_queue).jobs[(*job_queue).head];
    (*job_queue).head = ((*job_queue).head + 1) % (*job_queue).length;
    (*job_queue).size--;
    return pop;
}

void create_queue(Job_Queue *queue, int length)
{
    (*queue).jobs = (Job *)calloc(sizeof(Job), sizeof(Job) * length);
    if ((*queue).jobs == NULL) // make sure memory was allocated for the jobs
    {
        perror("ERROR: Calloc failed");
    }
    // set other basic queue components
    (*queue).tail = -1;
    (*queue).head = 0;
    (*queue).size = 0;
    (*queue).length = length;
    // initialise mutexes for adding and removing jobs from queue
    pthread_mutex_init(&(*queue).pop_mutex, NULL);
    pthread_mutex_init(&(*queue).add_mutex, NULL);
}

// TEST MODE FUNCTIONS

float RandomFloat(float min, float max) // get a random float between two float numbers (for random time delay)
{
    return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}

void *test_function(void *args)
{
    //printf("Starting New Thread\n");
    Thread_Args *thread_args = (Thread_Args *)args;
    Job_Queue *job_queue = thread_args->job_queue;
    Job to_do;
    while (1)
    {
        to_do.slot = -1;
        pthread_mutex_lock(&job_queue->pop_mutex);
        if (job_queue->size >= 1)
        {
            to_do = take_job(job_queue);
        }
        pthread_mutex_unlock(&job_queue->pop_mutex);
        if (to_do.slot == -1)
        {
            continue;
        }
        //printf("Job Slot: %d and Number: %ld \n", to_do.slot, to_do.num);
        for (int k = 0; k < 10; k++)
        {
            while (thread_args->shared_memory->server_flag[to_do.slot] == 1) // update the slot with the number only once client has read it
            {
            }
            printf("Run #: %d | Num: %lu\n", runcount, to_do.num + k);
            runcount++;
            srand((unsigned int)time(NULL));
            float randnum = RandomFloat(0.01, 0.1); // random delay
            sleep(randnum);
            pthread_mutex_lock(&thread_args->slot_mutex[to_do.slot]);      // mutex for the slot index
            thread_args->shared_memory->slots[to_do.slot] = to_do.num + k; // update the value in the slot with the factor
            pthread_mutex_unlock(&thread_args->slot_mutex[to_do.slot]);
            thread_args->shared_memory->server_flag[to_do.slot] = 1; // tell the client there is a new factor
        }
    }
}

void job_rotate(unsigned long int **rotations, int slot, unsigned long int number) // for test function
{
    unsigned long int new_number;
    for (int i = 0; i < 10; i++)
    {
        new_number = i * 10;
        rotations[slot][i] = new_number;
    }
}

// FINISHING
void finish_program(int handler)
{
    printf("\nFinishing Program...\n");
    printf("Clearing Shared Memory\n");
    shmctl(shm_id, IPC_RMID, NULL);
    printf("-=+=- Program Complete! -=+=-\n");
    exit(0);
}
