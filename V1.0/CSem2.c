//Counting Semaphore correct implementation
//McCrae Smith, Professor Ghosh, CMSC 312, 3/25/2021

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

typedef struct CSem
{
    int val;
    sem_t gate, mutex;
} csem;

void CSem(int K, struct CSem* sem)
{
    sem->val = K;
    if (sem->val > 0)
    {
	sem_init(&(sem->gate), 0, 1);
    }
    else
    {
	sem_init(&(sem->gate), 0, 0);
    }
    sem_init(&(sem->mutex), 0, 1);
    return;
}

void Pc(struct CSem* sem)
{
    sem_wait(&(sem->gate));
    sem_wait(&(sem->mutex));
    sem->val--;
    if (sem->val > 0)
    {
	sem_post(&(sem->gate));
    }
    sem_post(&(sem->mutex));
    return;
}

void Vc(struct CSem* sem)
{
    sem_wait(&(sem->mutex));
    sem->val++;
    if (sem->val == 1)
    {
	sem_post(&(sem->gate));
    }
    sem_post(&(sem->mutex));
    return;
}

void CSem_remove(struct CSem* sem)
{
    sem_destroy(&(sem->mutex));
    sem_destroy(&(sem->gate));
    return;
}
