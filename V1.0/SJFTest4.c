//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/15/2021
//Test 4, dynamically allocated queue, multiple threads
//Compile using gcc -lpthread SJFTest4.c -o SJFTest4

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "SJFGlobalQueue.c"

#define LOOPS 5
#define THREADS 10

struct Buffer *buf;

void *producer(void *thread)
{
    int threadID = *(int *)thread;
    int i;
    for(i = 0; i < LOOPS; i++)
    {
	int payload = (rand() % 901) + 100;
	sleep(rand() % 5);
	addBuffer(payload, threadID, buf);
	printf("Producer <%d> enqueue <%d> to buffer.\n", threadID, payload);
    }
    pthread_exit(0);
    return(0);
}

void *consumer(void *thread)
{
    int threadID = *(int *)thread;
    int i;
    for(i = 0; i < LOOPS; i++)
    {
	int result[2];
	sleep(rand() % 5);
	removeBuffer(result, buf);
	printf("Consumer <%d> dequeue <%d, %d> from buffer.\n", threadID, result[0], result[1]);
    }
    pthread_exit(0);
    return(0);
}

int main()
{
    buf = (struct Buffer *) malloc(sizeof(struct Buffer));
    initalizeBuffer(buf);
    printf("Buffer initalized, starting threads...\n");
    pthread_t thread[THREADS];
    int threadIDs[THREADS];
    int i = 0;
    while(i < THREADS)
    {
	threadIDs[i] = i;
	pthread_create(&thread[i], NULL, producer, &threadIDs[i]);
	i++;
	threadIDs[i] = i;
	pthread_create(&thread[i], NULL, consumer, &threadIDs[i]);
	i++;
    }
    
    for(i = 0; i < THREADS; i++)
    {
	pthread_join(thread[i], NULL);
    }

    printf("All jobs completed, average waiting time: %f\n", destroyBuffer(buf));
    free(buf);
    return(0);
}

