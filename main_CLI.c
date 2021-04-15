//***************************************************************************
//	Engineers:		Omar Amr, McCrae Smith, Papa Beye
//	Project:		CMSC 312 Assignment 3 - Group Project
//	Date:			04/26/2021
//***************************************************************************

#include "A3_support.c"
// #include "A3_support2.c"

struct node *n;
//counting sems
data *full_sem;
data *empty_sem;
//global array of nodes
job_info *buffer;
//LIFO buffer index
int *buffer_index;
//mutex to maintain mutual exclusion
pthread_mutex_t *buffer_mutex;

int main(int argc, int **argv)
{
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    int p_num, t_num;
    int shmid1, shmid2, shmid3, shmid4, shmid5;
    key_t key1, key2, key3, key4, key5;

    key1 = 123456789;
    key2 = 124681012;
    key3 = 135791113;
    key4 = 987654321;
    key5 = 147258369;

    //if there are not 2 arguments in CLI
    if (argc != 3)
    {
        printf("Sorry, you didn't input the correct number of arguments.\n");
        exit(1);
    }

////////////////////////////////////////////

    p_num = atoi(argv[1]);
    t_num = atoi(argv[2]);

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

    if ((shmid3 = shmget(key3, sizeof(pthread_mutex_t), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget_buffermutex");
        exit(1);
    }

    if ((shmid4 = shmget(key4, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget_emptysem");
        exit(1);
    }

    if ((shmid5 = shmget(key5, 25 * sizeof(node), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget_buffer");
        exit(1);
    }

    //attaching shm to our data space
    if ((full_sem = shmat(shmid1, NULL, 0)) == (char *)-1)
    {
        perror("shmat1");
        exit(1);
    }

    if ((empty_sem = shmat(shmid2, NULL, 0)) == (char *)-1)
    {
        perror("shmat2");
        exit(1);
    }

    if ((buffer_mutex = shmat(shmid3, NULL, 0)) == (char *)-1)
    {
        perror("shmat3");
        exit(1);
    }

    if ((buffer_index = shmat(shmid4, NULL, 0)) == (char *)-1)
    {
        perror("shmat3");
        exit(1);
    }

    if ((buffer = shmat(shmid5, NULL, 0)) == (char *)-1)
    {
        perror("shmat3");
        exit(1);
    }

    *buffer_index = 0;

    pthread_mutex_init(buffer_mutex, NULL);

    data_init(full_sem, SIZE);
    data_init(empty_sem, 0);

    int i,j,rand_jobs, rand_bytes;
    for (i = 0; i < p_num; i++) 
    {
        //enter new child process, start of producer
        if (fork() == 0)
        {
            srand(getpid());
            // rand_jobs = printRandoms(1, 25, 1);
            rand_jobs = (rand() % (25)) + 1;
            printf("rand_jobs: %d \n", rand_jobs);
            // for(j=0;j<rand_jobs;j++)
            // {
            //     rand_bytes = (rand() % (900 + 1)) + 100;
            //     printf("rand bytes: %d \n", rand_bytes);
            //     job_info new_info;
            //     new_info.pid = getpid();
            //     new_info.bytes = rand_bytes;
            //     insert(new_info);
            // }

            int wait;
            int j=0;
            // while (j++ < rand_jobs)
            for(j=0;j<rand_jobs;j++)
            {
                rand_bytes = (rand() % (900 + 1)) + 100;
                printf("rand bytes: %d \n", rand_bytes);
                job_info new_info;
                new_info.pid = getpid();
                new_info.bytes = rand_bytes;
                // sleep(rand() % 10);

                // sem_wait(&full_sem); // sem=0: wait. sem>0: go and decrement it
                /* possible race condition here. After this thread wakes up,
                another thread could aqcuire mutex before this one, and add to list.
                Then the list would be full again
                and when this thread tried to insert to buffer there would be
                a buffer overflow error */

                Pc(full_sem);

                pthread_mutex_lock(buffer_mutex); /* protecting critical section */
                wait = insertbuffer(new_info);
                if(wait)
                    j--;
                pthread_mutex_unlock(buffer_mutex);

                Vc(empty_sem);
                // sem_post(&empty_sem); // post (increment) emptybuffer semaphore
                printf("Producer %d added %d byte job to buffer\n",i, new_info.bytes);
            }

            // int cnt = count(head);
            // printf("count: %d, %d \n", cnt, j);
            // display(head);

            printf("[son] pid %d from [parent] pid %d\n", getpid(), getppid());
            exit(0);
        }
    }
    for (i = 0; i < p_num; i++) // loop will run n times (n=5)
        wait(NULL);

    int k;
    for(k=0;k<SIZE;k++)
        printf("element %02d, bytes: %04d, pid: %04d \n",k,buffer[k].bytes,buffer[k].pid);
    

    // int cnt = count(head);
    // printf("count: %d, %d \n", cnt, j);
    // display(head);

    shmctl(shmid5, IPC_RMID, NULL);

    ///////////////////////////////////////////////////////////////
    gettimeofday(&tv2, NULL);

    printf("Total time = %f seconds\n",
           (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
               (double)(tv2.tv_sec - tv1.tv_sec));

    return 0;
}

//inserts job into buffer, returns 0 on success, 1 on failure
int insertbuffer(job_info value)
{
    if (*buffer_index < SIZE)
    {
        buffer[(*buffer_index)++] = value;
        return 0;
    }
    else
    {
        printf("Buffer overflow\n");
        return 1;
    }
}