//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/18/2021
//Test 7, queue on Shared Memory, Producer Processes, Consumer Threads
//Compile using gcc -lpthread SJFTest7.c -o SJFTest7

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include "SJFGlobalQueue.c"

#define LOOPS 5
#define PROCESSES 5
#define THREADS 5


struct Buffer *buf;

void Producer(int ID, struct Buffer *buf)
{
	srand(getpid());
	int i;
	for(i = 0; i < LOOPS; i++)
    {
	int payload = (rand() % 901) + 100;
	sleep(rand() % 5);
	addBuffer(payload, ID, buf);
	printf("Producer <%d> enqueue <%d> to buffer.\n", ID, payload);
    }
    return;
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
	int shmIDBuf;
	key_t keyBuf;
	keyBuf = 8554;
	
	if((shmIDBuf = shmget(keyBuf, sizeof(struct Buffer), IPC_CREAT | 0666)) < 0)
	{
		perror("shmget Buf");
		exit(1);
	}
	if((buf = shmat(shmIDBuf, NULL, 0)) == (char *) -1)
	{
		perror("shmat Buf");
		exit(1);
	}
	
	initalizeBuffer(buf);
	printf("Buffer initalized, starting threads and forking processes...\n");
	
	pthread_t thread[THREADS];
    int threadIDs[THREADS];
	int i = 0;
	while(i < THREADS)
    {
		threadIDs[i] = i;
		pthread_create(&thread[i], NULL, consumer, &threadIDs[i]);
		i++;
    }
	i = 0;
	while (i < PROCESSES)
	{
		if(fork() == 0)
		{
			Producer(i++, buf);
			exit(0);
		}
		i++;
	}
	for (i = 0; i < PROCESSES; i++)
	{
		wait(NULL);
	}
	for(i = 0; i < THREADS; i++)
    {
		pthread_join(thread[i], NULL);
    }
	
	printf("All jobs complete, average waiting time: %f\n", destroyBuffer(buf));
	shmctl(shmIDBuf, IPC_RMID, NULL);
	return(0);
}
		
	
	
	
	
	
	
	
	
	