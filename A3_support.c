//***************************************************************************
//	Engineer:		Omar Amr
//	Project:		CMSC 312 Assignment 2
//	Date:			03/26/2021
//***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define SIZE 5
#define NUMB_THREADS 6
#define PRODUCER_LOOPS 2

struct data
{
    int val;
    sem_t gate;
    sem_t mutex;
};

typedef struct data data;

void data_init(data *d, int k)
{
    d->val = k;
    int init_val = d->val > 0 ? 1 : 0;
    sem_init(&d->gate,  // sem_t *sem
             0,         // int pshared. 0 = shared between threads of process,  1 = shared between processes
             init_val); // unsigned int value. Initial value
    sem_init(&d->mutex,
             0,
             1);
}

void Pc(data *cs)
{
    sem_wait(&cs->gate);
    sem_wait(&cs->mutex);
    cs->val--;
    if (cs->val > 0)
        sem_post(&cs->gate);

    sem_post(&cs->mutex);
}

void Vc(data *cs)
{
    sem_wait(&cs->mutex);
    cs->val++;
    if (cs->val == 1)
        sem_post(&cs->gate);

    sem_post(&cs->mutex);
}