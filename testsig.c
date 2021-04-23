//SJF Global Queue testing  signal handler program
//McCrae Smith & Papa Beye, Professor Ghosh, CMSC 312, 4/20/2021
//Full version, all assignment features included
// make && ./app NUMPROCESSES NUMTHREADS 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "task2/sjfqueue.h"
#include "signal_handler.h"

struct Buffer *buf; //SJF buffer
pthread_t *threads;
int *processIDs;
int threadNum, processNum; 
int shmIDBuf;

//Producer process function, adds up to 25 jobs of random size between 100 and 1000
void Producer(int ID)
{
	srand(getpid()); //Seed RNG with process ID, unique for each producer
	int i;
	int loops = (rand() % 25) + 1; //Do a random number of jobs between 1 and 25
	for(i = 0; i < loops; i++)
    {
		int payload = (rand() % 901) + 100; //Create a random length for the job between 100 and 1000
		usleep(payload * 1000); //Wait for 0.1 to 1 second
		addBuffer(payload, getpid(), buf); //Put in buffer
		printf("Producer <%d> enqueue <%d> to buffer.\n", getpid(), payload);
    }
    return;
}

//Consumer thread function, removes jobs continuously from queue
void *consumer(void *thread)
{
    int threadID = *(int *)thread;
    while(1) //Do continously
    {
		int result[2];
		removeBuffer(result, buf); //Remove the first job from the buffer, the shortest job currently
		printf("Consumer <%d> dequeue <%d, %d> from buffer.\n", threadID, result[0], result[1]);
		usleep(result[1]*1000); //Wait for 0.1 to 1 second
    }
    pthread_exit(0); //Never actually reached
    return(0);
}

//Simple signal handler to catch SIGINT/Ctrl+C


//Main/control process
int main(int argc, char*argv[])
{
	//int mainStopwatch = (int) time(NULL);
	struct timespec mainStopwatch, timeNow;
	clock_gettime(CLOCK_MONOTONIC, &mainStopwatch); //Start stopwatch for total running time
	double timeResult;
	printf("Program started pid=%d, stopwatch started, initalizing buffer...\n", getpid());
  sleep(2); // sleep 2 for record pid value for testing sigs
	key_t keyBuf;
	keyBuf = 8555;
	if(argc != 3)
	{
		perror("params");
		exit(1);
	}
	processNum = atoi(argv[1]);
	threadNum = atoi(argv[2]);
	
	if((shmIDBuf = shmget(keyBuf, sizeof(struct Buffer), IPC_CREAT | 0666)) < 0) //Get shared memory segment
	{
		perror("shmget Buf");
		exit(1);
	}
	if((buf = shmat(shmIDBuf, NULL, 0)) == (char *) -1) //Attach shared memory segment
	{
		perror("shmat Buf");
		exit(1);
	}
	
	initalizeBuffer(buf); //Initalize the buffer
	printf("Buffer initalized, starting threads and forking processes...\n");
	
	threads = (pthread_t *) malloc(sizeof(pthread_t) * threadNum);
	processIDs = (int *) malloc(sizeof(int) * processNum);
	//pthread_t threads[threadNum];
	int threadIDs[threadNum];
	//int processIDs[processNum];
	int i = 0;
	while(i < threadNum) //Create the consumer threads
    {
		threadIDs[i] = i;
    		pthread_create(&threads[i], NULL, consumer, &threadIDs[i]);
		i++;
    }
	i = 0;
	while (i < processNum) //Create the producer processes
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

  sig_dt sigmsg;
  sigmsg.prcs = processIDs;
  sigmsg.thrdsId = threadIDs;
  sigmsg.thrds = threads;
  sigmsg._numTh = threadNum;
  sigmsg._numPr = processNum;
  sigmsg.shmID =  shmIDBuf;
	init_sig_hander(&sigmsg); // register signals
	for (i = 0; i < processNum; i++) //Clean up the producer processes
	{
		wait(NULL);
	}
	isEmpty(buf);
	for(i = 0; i < threadNum; i++) //Shut down and clean up the consumer threads
    {
		pthread_cancel(threads[i]);
		pthread_join(threads[i], NULL);
    }
	
	printf("All jobs complete, average waiting time: %f\n", destroyBuffer(buf)); //Get average waiting time and destroy the buffer
	free(threads);
	free(processIDs);
	shmctl(shmIDBuf, IPC_RMID, NULL); //Remove the shared memory
	//mainStopwatch = (int) time(NULL) - mainStopwatch;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	timeResult = (double)(timeNow.tv_sec - mainStopwatch.tv_sec) + ((double)(timeNow.tv_nsec - mainStopwatch.tv_nsec)) / 1000000000; //Find the total execution time
	printf("Program completed, total processing time: %f\n", timeResult);
	return(0); //Done
}
	
	
	
	
	
	
	
	
	
	
