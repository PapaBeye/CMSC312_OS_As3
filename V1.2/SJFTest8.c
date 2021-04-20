//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/18/2021
//Test 8, Testing assignment scenario
//Compile using gcc -lpthread SJFTest8.c -o SJFTest8

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include "SJFGlobalQueue.c"

struct Buffer *buf;
//pthread_t *thread;
//int exitFlag, termFlag, threads;

void Producer(int ID, struct Buffer *buf)
{
	srand(getpid());
	int i;
	int loops = (rand() % 25) + 1;
	for(i = 0; i < loops; i++)
    {
		int payload = (rand() % 901) + 100;
		usleep(payload * 1000);
		addBuffer(payload, getpid(), buf);
		printf("Producer <%d> enqueue <%d> to buffer.\n", getpid(), payload);
    }
    return;
}

void *consumer(void *thread)
{
    int threadID = *(int *)thread;
    while(1)
    {
		int result[2];
		removeBuffer(result, buf);
		printf("Consumer <%d> dequeue <%d, %d> from buffer.\n", threadID, result[0], result[1]);
    }
    pthread_exit(0);
    return(0);
}

int main(int argc, char*argv[])
{
	int mainStopwatch = (int) time(NULL);
	printf("Program started, stopwatch started, initalizing buffer...\n");
	int shmIDBuf;
	key_t keyBuf;
	keyBuf = 8554;
	if(argc != 3)
	{
		perror("params");
		exit(1);
	}
	int processes = atoi(argv[1]);
	threads = atoi(argv[2]);
	
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
	
	//threads = (pthread_t *) malloc(sizeof(pthread_t) * threads);
	pthread_t threads[threads];
    int threadIDs[threads];
	int processIDs[processes];
	int i = 0;
	while(i < threads)
    {
		threadIDs[i] = i;
		pthread_create(&thread[i], NULL, consumer, &threadIDs[i]);
		i++;
    }
	i = 0;
	while (i < processes)
	{
		pid = fork();
		if(pid == 0)
		{
			Producer(i++, buf);
			exit(0);
		}
		else
		{
			processIDs[i] = pid;
		}
		i++;
	}
	for (i = 0; i < processes; i++)
	{
		wait(NULL);
	}
	isEmpty(buf);
	for(i = 0; i < threads; i++)
    {
		pthread_cancel(thread[i]);
		pthread_join(thread[i], NULL);
    }
	
	printf("All jobs complete, average waiting time: %f\n", destroyBuffer(buf));
	free(threads)
	shmctl(shmIDBuf, IPC_RMID, NULL);
	mainStopwatch = (int) time(NULL) - mainStopwatch;
	printf("Program completed, total processing time: %d\n", mainStopwatch);
	return(0);
}
	
	
	
	
	
	
	
	
	
	