//***************************************************************************
//	Engineers:		Omar Amr, McCrae Smith, Papa Beye
//	Project:		CMSC 312 Assignment 3 - Group Project
//	Date:			04/23/2021
//  Description:    Main file for the LIFO implmentation of print server
//***************************************************************************

#include "A3_support.c"

//counting sems
data *full_sem;
data *empty_sem;
//global array of nodes
job_info *buffer;
//LIFO buffer index
int *buffer_index;
//mutex to maintain mutual exclusion of buffer
sem_t *buffer_mutex;
//flag to thread exit
int exit_flag;
//variables to all threads
int p_num, t_num;
pthread_t *thread; //array of threads
int *thread_numb;  //array of thread nums
//total number of rand_jobs
int total_jobs;
//total waiting time
double total_wait;
//shared memory seg uds
int shmid1, shmid2, shmid3, shmid4, shmid5;
//array of process ids to cancel early
int *processIDs;
//keeps track of how many forks have been done, always <= p_tum
int produced;

//function stubs
void insertbuffer(job_info value);
job_info dequeuebuffer();
void *consumer(void *thread_n);
void isEmpty();
void free_all();
void sig_handler(int sig);

int main(int argc, int **argv)
{
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    key_t key1, key2, key3, key4, key5;

    key1 = 123456789;
    key2 = 124681012;
    key3 = 135791113;
    key4 = 987654321;
    key5 = 147258369;
    total_wait = 0;
    total_jobs = 0;

    //if there are not 2 arguments in CLI
    if (argc != 3)
    {
        printf("Sorry, you didn't input the correct number of arguments.\n");
        exit(1);
    }

    ////////////////////////////////////////////

    p_num = atoi((char *)argv[1]);
    t_num = atoi((char *)argv[2]);

    //alloc dyn mem for arrays
    thread = (pthread_t *)malloc(t_num * sizeof(pthread_t));
    thread_numb = (int *)malloc(t_num * sizeof(int));
    processIDs = (int *)malloc(p_num * sizeof(int));

    //setting up shared memory for global vars
    if ((shmid1 = shmget(key1, sizeof(data), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget_fullsem");
        exit(1);
    }

    if ((shmid2 = shmget(key2, sizeof(data), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget_emptysem");
        exit(1);
    }

    if ((shmid3 = shmget(key3, sizeof(sem_t), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget_buffermutex");
        exit(1);
    }

    if ((shmid4 = shmget(key4, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget_bufferindex");
        exit(1);
    }

    if ((shmid5 = shmget(key5, SIZE * sizeof(job_info), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget_buffer");
        exit(1);
    }

    //attaching shm to our data space
    if ((full_sem = shmat(shmid1, NULL, 0)) == (data *)-1)
    {
        perror("shmat1");
        exit(1);
    }

    if ((empty_sem = shmat(shmid2, NULL, 0)) == (data *)-1)
    {
        perror("shmat2");
        exit(1);
    }

    if ((buffer_mutex = shmat(shmid3, NULL, 0)) == (sem_t *)-1)
    {
        perror("shmat3");
        exit(1);
    }

    if ((buffer_index = shmat(shmid4, NULL, 0)) == (int *)-1)
    {
        perror("shmat3");
        exit(1);
    }

    if ((buffer = shmat(shmid5, NULL, 0)) == (job_info *)-1)
    {
        perror("shmat3");
        exit(1);
    }

    //initalizing variables for LIFO queue
    *buffer_index = 0;

    //initalize buffer mutex
    sem_init(buffer_mutex, // sem_t *sem
             1,            // int pshared. 0 = shared between threads of process,  1 = shared between processes
             1);

    //init our CSemaphores
    data_init(full_sem, SIZE);
    data_init(empty_sem, 0);

    //creating threads
    int i;
    for (i = 0; i < t_num;)
    {
        thread_numb[i] = i;
        // playing a bit with thread and thread_numb pointers...
        pthread_create(&thread[i],       // pthread_t *t
                       NULL,             // const pthread_attr_t *attr
                       consumer,         // void *(*start_routine) (void *)
                       &thread_numb[i]); // void *arg
        i++;
    }

    srand(getpid());

    //forking processes
    int rand_jobs, rand_bytes, pid;
    for (i = 0; i < p_num; i++)
    {
        pid = fork();
        produced++;
        //enter new child process, start of producer function
        if (pid == 0)
        {
            srand(getpid());
            rand_jobs = printRandoms(1, 25, 1);
            // printf("rand_jobs: %d \n", rand_jobs);

            int j = 0;
            for (j = 0; j < rand_jobs; j++)
            {
                rand_bytes = (rand() % (900 + 1)) + 100;
                // printf("rand bytes: %d \n", rand_bytes);
                job_info new_info;
                new_info.pid = getpid();
                new_info.bytes = rand_bytes;
                gettimeofday(&(new_info.stopwatch), NULL); //get current time

                // printf("csem: %d \n", full_sem->val);
                // printf("csem: %d \n", empty_sem->val);
                Pc(full_sem);

                sem_wait(buffer_mutex); /* protecting critical section */
                insertbuffer(new_info); //place job into buffer
                sem_post(buffer_mutex);

                Vc(empty_sem); // post (increment) emptybuffer semaphore
                printf("Producer <%d> added <%d> to buffer\n", i, new_info.bytes);
                usleep(new_info.bytes * 1000);
            }

            // printf("[son] pid %d from [parent] pid %d\n", getpid(), getppid());
            exit(0);
        }

        else
        {
            processIDs[i] = pid;
            // printf("produced pid: %d, prod var: %d \n", pid, produced);
        }

        // wait(NULL);
    }

    signal(SIGINT, sig_handler); //Register the signal handler

    //wait for each child process to exit
    for (i = 0; i < p_num; i++)
        wait(NULL);

    //set flag to allow threads to exit when queue
    exit_flag = 1;

    //polling while loop to wait up there are no more jobs on queue, thus threads are done and can be cancelled
    isEmpty();

    for (i = 0; i < t_num; i++)
        pthread_join(thread[i], NULL);

    // int k;
    // for(k=0;k<SIZE;k++)
    //     printf("element %02d, bytes: %04d, pid: %04d \n",k,buffer[k].bytes,buffer[k].pid);

    printf("############################################################\n");
    // printf("flag: %d, index: %d \n", exit_flag, *buffer_index);
    printf("total jobs placed on to queue: %d \n", total_jobs);
    printf("total jobs removed from queue: %d \n", total_jobs);
    // printf("total wait time: %f \n", total_wait);
    printf("All jobs completed, average waiting time: %f seconds \n", total_wait / total_jobs);

    free_all();

    ///////////////////////////////////////////////////////////////
    gettimeofday(&tv2, NULL);

    printf("Program completed, total processing time = %f seconds\n",
           (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
               (double)(tv2.tv_sec - tv1.tv_sec));

    return 0;
}

//inserts job into buffer, returns 0 on success, 1 on failure
void insertbuffer(job_info value)
{
    if (*buffer_index < SIZE)
    {
        buffer[(*buffer_index)++] = value;
        // return 0;
    }
    else
    {
        printf("Buffer overflow\n");
        // return 1;
    }
}

job_info dequeuebuffer()
{
    if (*buffer_index > 0)
    {
        return buffer[--(*buffer_index)]; // buffer_index-- would be error!
    }
    else
    {
        printf("Buffer underflow\n");
    }
    job_info n;
    return n;
}

void *consumer(void *thread_n)
{
    int thread_numb = *(int *)thread_n;
    int i = 0;
    int rand;
    job_info ret;
    struct timeval tv2;

    while (1)
    {
        Pc(empty_sem);

        /* there could be race condition here, that could cause
           buffer underflow error */
        sem_wait(buffer_mutex);
        ret = dequeuebuffer();
        total_jobs++;
        gettimeofday(&tv2, NULL);
        total_wait += (double)(tv2.tv_usec - ret.stopwatch.tv_usec) / 1000000 +
                      (double)(tv2.tv_sec - ret.stopwatch.tv_sec);

        sem_post(buffer_mutex);

        Vc(full_sem);
        printf("Consumer <%d> dequeue <%d,%d> from buffer\n", thread_numb, ret.pid, ret.bytes);
        // printf("Buffer index: %d \n", *buffer_index);
        usleep(ret.bytes * 1000);
    }
    pthread_exit(0);
}

void isEmpty()
{

    while (1)
    {
        if (*buffer_index == 0)
        {
            // printf("entered break for thread %d, flag = %d\n", *thread_numb, exit_flag);
            int h;
            for (h = 0; h < t_num; h++)
            {
                pthread_cancel(thread[h]);
            }
            // pthread_exit(0);
            break;
        }
    }
};

void free_all()
{

    free(thread_numb);
    free(thread);
    free(processIDs);

    sem_destroy(&full_sem->gate);
    sem_destroy(&full_sem->mutex);
    sem_destroy(&empty_sem->gate);
    sem_destroy(&empty_sem->mutex);
    sem_destroy(buffer_mutex);

    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    shmctl(shmid4, IPC_RMID, NULL);
    shmctl(shmid5, IPC_RMID, NULL);
}

//Simple signal handler to catch SIGINT/Ctrl+C
void sig_handler(int sig)
{
    int i;
    printf("\nSIGINT caught, terminating program...\n");
    for (i = 0; i < produced; i++) //Kill and clean up the producer processes
    {
        kill(processIDs[i], SIGINT);
        waitpid(processIDs[i], NULL, 1);
    }
    for (i = 0; i < t_num; i++) //Shut down and clean up the consumer threads
    {
        pthread_cancel(thread[i]);
        pthread_join(thread[i], NULL);
    }
    free_all();

    exit(1); //Quit the program, do not return to main
}
