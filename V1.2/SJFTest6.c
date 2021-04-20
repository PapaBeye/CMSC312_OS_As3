//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/18/2021
//Test 6, queue on Shared Memory, Multiple Processes
//Compile using gcc -lpthread SJFTest6.c -o SJFTest6

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "SJFGlobalQueue.c"

#define LOOPS 5
#define PROCESSES 10

void Producer(int ID, struct Buffer *buf)
{
	srand(getpid());
	int i;
	for(i = 0; i < LOOPS; i++)
    {
	int payload = (rand() % 901) + 100;
	sleep(rand() % 5);
	addBuffer(payload, threadID, buf);
	printf("Producer <%d> enqueue <%d> to buffer.\n", ID, payload);
    }
    return;
}

void Consumer(int ID, struct Buffer *buf)
{
	srand(getpid());
	int i;
    for(i = 0; i < LOOPS; i++)
    {
	int result[2];
	sleep(rand() % 5);
	removeBuffer(result, buf);
	printf("Consumer <%d> dequeue <%d, %d> from buffer.\n", ID, result[0], result[1]);
    }
	return;
}

int main()
{
	int shmIDBuf;
	key_t keyBuf;
	struct Buffer *buf;
	
	keyBuf = 8553;
	
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
	printf("Buffer initalized, forking processes...\n");
	int i = 0;
	while (i < PROCESSES)
	{
		if(fork() == 0)
		{
			Producer(i++, buf);
			exit(0);
		}
		i++;
		if(fork() == 0)
		{
			Consumer(i++, buf);
			exit(0);
		}
		i++;
	}
	for (i = 0; i < PROCESSES; i++)
	{
		wait(NULL);
	}
	
	printf("All jobs complete, average waiting time: %f\n", destroyBuffer(buf));
	shmctl(shmIDBuf, IPC_RMID, NULL);
	return(0);
}
		
	
	
	
	
	
	
	
	
	