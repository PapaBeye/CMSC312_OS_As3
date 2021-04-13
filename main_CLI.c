//***************************************************************************
//	Engineers:		Omar Amr, McCrae Smith, Papa Beye
//	Project:		CMSC 312 Assignment 3 - Group Project
//	Date:			04/26/2021
//***************************************************************************

#include "A3_support.c"
#include "A3_support2.c"

int main(int argc, int **argv)
{

    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    buffer_index = 0;

    pthread_mutex_init(&buffer_mutex, NULL);

    data_init(&full_sem, SIZE);
    data_init(&empty_sem, 0);

    // sem_init(&full_sem, // sem_t *sem
    //          0, // int pshared. 0 = shared between threads of process,  1 = shared between processes
    //          SIZE); // unsigned int value. Initial value
    // sem_init(&empty_sem,
    //          0,
    //          0);
    /* full_sem is initialized to buffer size because SIZE number of
       producers can add one element to buffer each. They will wait
       semaphore each time, which will decrement semaphore value.
       empty_sem is initialized to 0, because buffer starts empty and
       consumer cannot take any element from it. They will have to wait
       until producer posts to that semaphore (increments semaphore
       value) */
    pthread_t thread[NUMB_THREADS];
    int thread_numb[NUMB_THREADS];
    int i;
    for (i = 0; i < NUMB_THREADS;)
    {
        thread_numb[i] = i;
        pthread_create(thread + i,       // pthread_t *t
                       NULL,             // const pthread_attr_t *attr
                       producer,         // void *(*start_routine) (void *)
                       thread_numb + i); // void *arg
        i++;
        thread_numb[i] = i;
        // playing a bit with thread and thread_numb pointers...
        pthread_create(&thread[i],       // pthread_t *t
                       NULL,             // const pthread_attr_t *attr
                       consumer,         // void *(*start_routine) (void *)
                       &thread_numb[i]); // void *arg
        i++;
    }

    for (i = 0; i < NUMB_THREADS; i++)
        pthread_join(thread[i], NULL);

    pthread_mutex_destroy(&buffer_mutex);
    // sem_destroy(&full_sem);
    // sem_destroy(&empty_sem);
    sem_destroy(&full_sem.gate);
    sem_destroy(&full_sem.mutex);
    sem_destroy(&empty_sem.gate);
    sem_destroy(&empty_sem.mutex);

    gettimeofday(&tv2, NULL);

    printf("Total time = %f seconds\n",
           (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
               (double)(tv2.tv_sec - tv1.tv_sec));

    return 0;
}