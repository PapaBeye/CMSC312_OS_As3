//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/15/2021
//Test 2, local queue, multiple entries
//Compile using gcc -lpthread SJFTest2.c -o SJFTest2

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
	return(0);
}
	
