//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/15/2021
//Test 1, local queue, single entry
//Compile using gcc -lpthread SJFTest1.c -o SJFTest1

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "SJFGlobalQueue.c"

int main()
{
	struct Buffer buffer;
	struct Buffer *buf = &buffer;
	int result[2];
	
	initalizeBuffer(buf);
	addBuffer(12, 1, buf);
	printf("Producer <1> added <12> to buffer.\n");
	removeBuffer(result, buf);
	printf("Consumer <1> dequeue <%d, %d> from buffer.\n", result[0], result[1]); 
	printf("Total time waiting: %f seconds\n", destroyBuffer(buf));
	printf("All jobs processed, exiting...\n");
	return(0);
}
	
