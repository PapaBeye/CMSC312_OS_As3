//SJF Global Queue testing program
//McCrae Smith, Professor Ghosh, CMSC 312, 4/15/2021
//Test 3, local queue, random value stress testing
//Compile using gcc -lpthread SJFTest3.c -o SJFTest3

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "SJFGlobalQueue.c"

int main()
{
	srand(time(NULL));
	struct Buffer buffer;
	struct Buffer *buf = &buffer;
	int result[2];
	int loops = rand() % 25;
	int i;
	initalizeBuffer(buf);

	for(i = 0; i < loops; i++)
	{
	    int data = (rand() % 901) + 100;
	    int prodID = (rand() % 6) + 1;
	    addBuffer(data, prodID, buf);
	    printf("Producer <%d> enqueue <%d> to buffer.\n", prodID, data);
	}
	for(i = 0; i < loops; i++)
	{
	    int consID = (rand() % 6) + 1;
	    removeBuffer(result, buf);
	    printf("Consumer <%d> dequeue <%d, %d> from buffer.\n", consID, result[0], result[1]);
	}

	printf("Total time waiting: %f seconds\n", destroyBuffer(buf));
	printf("All jobs processed, exiting...\n");
	return(0);
}
	
