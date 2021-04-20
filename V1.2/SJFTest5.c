//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/18/2021
//Test 5, queue on Shared Memory, single processe
//Compile using gcc -lpthread SJFTest5.c -o SJFTest5

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "SJFGlobalQueue.c"

int main()
{
	int shmIDBuf;
	key_t keyBuf;
	struct Buffer *buf;
	
	keyBuf = 8552;
	
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
	addBuffer(12, 1, buf);
	printf("Producer <1> added <12> to buffer.\n");
	addBuffer(10, 2, buf);
	printf("Producer <2> added <10> to buffer.\n");
	addBuffer(20, 1, buf);
	printf("Producer <1> added <20> to buffer.\n");
	removeBuffer(result, buf);
	printf("Consumer <2> dequeue <%d, %d> from buffer.\n", result[0], result[1]);
	addBuffer(16, 2, buf);
	printf("Producer <2> added <16> to buffer.\n");
	removeBuffer(result, buf);
	printf("Consumer <1> dequeue <%d, %d> from buffer.\n", result[0], result[1]); 
	removeBuffer(result, buf);
	printf("Consumer <2> dequeue <%d, %d> from buffer.\n", result[0], result[1]);
	removeBuffer(result, buf);
	printf("Consumer <1> dequeue <%d, %d> from buffer.\n", result[0], result[1]);
	printf("Total time waiting: %f seconds\n", destroyBuffer(buf));
	printf("All jobs processed, exiting...\n");
	shmctl(shmIDBuf, IPC_RMID, NULL);
	exit(0);
}