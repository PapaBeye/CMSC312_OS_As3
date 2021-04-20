//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/20/2021
//Test 8, Testing assignment scenario, no signal handler
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
//int *processIDs;
//int exitFlag, termFlag;
int threadNum, processNum;

void Producer(int ID)
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
		usleep(result[1]*1000);
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
	processNum = atoi(argv[1]);
	threadNum = atoi(argv[2]);
	
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
	pthread_t threads[threadNum];
	int threadIDs[threadNum];
	int processIDs[processNum];
	int i = 0;
	while(i < threadNum)
    {
		threadIDs[i] = i;
    		pthread_create(&threads[i], NULL, consumer, &threadIDs[i]);
		i++;
    }
	i = 0;
	while (i < processNum)
	{
		int pid = fork();
		if(pid == 0)
		{
			Producer(i++);
			exit(0);
		}
		else
		{
			processIDs[i] = pid;
		}
		i++;
	}
	for (i = 0; i < processNum; i++)
	{
		wait(NULL);
	}
	isEmpty(buf);
	for(i = 0; i < threadNum; i++)
    {
		pthread_cancel(threads[i]);
		pthread_join(threads[i], NULL);
    }
	
	printf("All jobs complete, average waiting time: %f\n", destroyBuffer(buf));
	//free(threads);
	shmctl(shmIDBuf, IPC_RMID, NULL);
	mainStopwatch = (int) time(NULL) - mainStopwatch;
	printf("Program completed, total processing time: %d\n", mainStopwatch);
	return(0);
}
	
	
	
	
	
	
	
	
	
	
