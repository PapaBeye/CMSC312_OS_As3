//*
//McCrae Smith
//Professor Ghosh
//CMSC 312
//4/23/2021

//Assignment 3, Task 2, SJF Implementation

//TODO:
// 1 - Introduce binary semaphore array for extra credit task (if possible)
// 2 - Modify stopwatch to track milliseconds (if possible)
// 3 - Review code and comments for clarity and remove unused lines (if possible)

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include "CSem2.c"

#define POOLSIZE 25

struct Node
{
	int data; //Job size
	int process; //Process that created job
	struct Node *next; //Next node in sequence
	int valid; //Boolean if node is safe to overwrite
	int stopwatch; //Stopwatch varable to track waiting time, in seconds
};

struct Buffer
{
	sem_t mutex; //Buffer mutex
	sem_t emptyGate; //Binary sem, gate for checking empty buffer
	struct CSem full_sem; //sem_t full_sem; //When 0, buffer is full
	struct CSem empty_sem; //sem_t empty_sem; //When 0, buffer is empty
	struct Node array[POOLSIZE]; //Buffer array, nodes will be ordered in linked list
	struct Node *head; //Pointer to first element of list
	int size; //Counter for number of jobs
	int totalWaiting; //Total time spent waiting in the buffer, in seconds
	int jobs; //Total number of jobs put on the buffer
};

void initalizeBuffer(struct Buffer *buffer)
{
	//Initalize varables of Buffer struct as empty buffer
	sem_init(&(buffer->mutex), 1, 1);
	sem_init(&(buffer->emptyGate), 1, 1);
	CSem(POOLSIZE, &(buffer->full_sem)); //sem_init(&(buffer->full_sem), 1, POOLSIZE);
	CSem(0, &(buffer->empty_sem)); //sem_init(&(buffer->empty_sem), 1, 0);
	buffer->head = NULL;
	buffer->totalWaiting = 0;

	//Initalize elements of buffer array
	int i;
	for (i = 0; i < POOLSIZE; i++)
	{
		buffer->array[i].data = 0;
		buffer->array[i].process = 0;
		buffer->array[i].next = NULL;
		buffer->array[i].valid = 0;
	}
	return;
}

//Add new node in SJF order to buffer
void addBuffer(int payload, int processID, struct Buffer *buffer)
{
	//Build new node
	struct Node new;
	new.data = payload;
	new.process = processID;
	new.next = NULL;
	new.valid = 1;
	new.stopwatch = (int) time(NULL);
	
	Pc(&(buffer->full_sem)); //sem_wait(&(buffer->full_sem)); //Decrement full semaphore
	sem_wait(&(buffer->mutex)); //Lock Mutex
	int i = 0;
	
	//Find the first invalid (can be replaced) entry in array
	while (buffer->array[i].valid == 1 && i < POOLSIZE)
	{
		i++;
	}
	buffer->array[i] = new; //Put new node in array
	
	//Place node in the linked list
	struct Node *curr = buffer->head;
	if (curr == NULL) //If head is null
	{
		buffer->head = &(buffer->array[i]);
	}
	else if(curr->data > buffer->array[i].data)//If head > new
	{
	    buffer->array[i].next = curr;
	    buffer->head = &(buffer->array[i]); //Make new head and point to old head
	}
	else //Otherwise new goes after first node
	{
		//Insert node such that prevous node is less than or equal, and next is more than
		struct Node *next = curr->next;
		int notFound = 1;
		while (next != NULL && notFound) //While not at the end of the list, and location is not found
		{
			if (next->data > buffer->array[i].data) //If current <= new < next
			{
				notFound = 0;
			}
			else //Else move along list
			{
				curr = next;
				next = curr->next;
			}
		}
		//Link to list
		curr->next = &(buffer->array[i]);
		buffer->array[i].next = next;
	}
	
	//Increment total jobs and size
	buffer->jobs = buffer->jobs + 1;
	buffer->size = buffer->size + 1;
	
	//If the buffer is no longer empty, lock empty gate
	if (buffer->size == 1)
	{
		sem_wait(&(buffer->emptyGate));
	}
	
	//Increment empty semaphore, and release mutex
	sem_post(&(buffer->mutex));
	Vc(&(buffer->empty_sem)); //sem_post(&(buffer->empty_sem));
	
	return;
}

//Remove first node (shortest job) from buffer, return payload and process ID
void removeBuffer(int result[2], struct Buffer *buffer)
{
	//Decrement empty semaphore, and lock mutex
	Pc(&(buffer->empty_sem)); //sem_wait(&(buffer->empty_sem));
	sem_wait(&(buffer->mutex));
	
	result[1] = buffer->head->data; //Retreve value
	result[0] = buffer->head->process; //Retreve process
	buffer->head->valid = 0; //Allow node to be overwritten
	buffer->totalWaiting = buffer->totalWaiting + ((int) time(NULL) - buffer->head->stopwatch); //Calculate time spent waiting on buffer, add to total
	buffer->head = buffer->head->next; //Delink from list
	buffer->size = buffer->size - 1; //Decrement size
	
	//If the buffer is now empty, release empty gate
	if (buffer->size == 0)
	{
		sem_post(&(buffer->emptyGate));
	}
	
	//Increment full semaphore, and release mutex
	sem_post(&(buffer->mutex));
	Vc(&(buffer->full_sem)); //sem_post(&(buffer->full_sem));
	
	return; //Return process ID that added job, and payload
}

//Destroys the semaphores and returns the average waiting time
double destroyBuffer(struct Buffer *buffer)
{
    sem_destroy(&(buffer->mutex));//Destroy mutex lock
    CSem_remove(&(buffer->full_sem)); //sem_destroy(&(buffer->full_sem));//Destroy full semaphore
    CSem_remove(&(buffer->empty_sem)); //sem_destroy(&(buffer->empty_sem));//Destroy empty semaphore
    return((double) buffer->totalWaiting / (double) buffer->jobs); //Divide total waiting time of all jobs by number of jobs for average waiting time
}

//Returns the current size of the buffer
int getBufferCount(struct Buffer *buffer)
{
	sem_wait(&(buffer->mutex));
	return buffer->size;
	sem_post(&(buffer->mutex));
}

//Wait on emptyGate, use only after all addBuffer calls have been used
void isEmpty(struct Buffer *buffer)
{
	sem_wait(&(buffer->emptyGate));
	sem_wait(&(buffer->mutex));
	sem_post(&(buffer->mutex));
	sem_post(&(buffer->emptygate));
	return;
}
