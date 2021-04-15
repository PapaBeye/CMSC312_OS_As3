//***************************************************************************
//	Engineer:		Omar Amr
//	Project:		CMSC 312 Assignment 2
//	Date:			03/26/2021
//***************************************************************************
#ifndef supported
#define supported

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SIZE 25
#define NUMB_THREADS 6
#define PRODUCER_LOOPS 2

struct data
{
    int val;
    sem_t gate;
    sem_t mutex;
};

typedef struct data data;

struct job_info
{
    data csem;
    pid_t pid;
    int bytes;
};
typedef struct job_info job_info;

void data_init(data *d, int k)
{
    d->val = k;
    int init_val = d->val > 0 ? 1 : 0;
    sem_init(&d->gate,  // sem_t *sem
             1,         // int pshared. 0 = shared between threads of process,  1 = shared between processes
             init_val); // unsigned int value. Initial value
    sem_init(&d->mutex,
             1,
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

////////////////////////////////////////////////////////////////////

struct node
{
    job_info info;
    struct node *next;
}*head;

typedef struct node node;

void append(job_info info)
{
    struct node *temp, *right;
    temp = (struct node *)malloc(sizeof(struct node));
    temp->info = info;
    right = (struct node *)head;
    while (right->next != NULL)
        right = right->next;
    right->next = temp;
    right = temp;
    right->next = NULL;
}

void add(job_info info)
{
    struct node *temp;
    temp = (struct node *)malloc(sizeof(struct node));
    temp->info = info;
    if (head == NULL)
    {
        head = temp;
        head->next = NULL;
    }
    else
    {
        temp->next = head;
        head = temp;
    }
}
void addafter(job_info info, int loc)
{
    int i;
    struct node *temp, *left, *right;
    right = head;
    for (i = 1; i < loc; i++)
    {
        left = right;
        right = right->next;
    }
    temp = (struct node *)malloc(sizeof(struct node));
    temp->info = info;
    left->next = temp;
    left = temp;
    left->next = right;
    return;
}

void insert(job_info new_info)
{
    int c = 0;
    struct node *temp;
    temp = head;
    if (temp == NULL)
    {
        add(new_info);
    }
    else
    {
        while (temp != NULL)
        {
            if (temp->info.bytes < new_info.bytes)
                c++;
            temp = temp->next;
        }
        if (c == 0)
            add(new_info);
        else if (c < count())
            addafter(new_info, ++c);
        else
            append(new_info);
    }
}

int delete (int num)
{
    struct node *temp, *prev;
    temp = head;
    while (temp != NULL)
    {
        if (temp->info.bytes == num)
        {
            if (temp == head)
            {
                head = temp->next;
                free(temp);
                return 1;
            }
            else
            {
                prev->next = temp->next;
                free(temp);
                return 1;
            }
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }
    return 0;
}

void display(struct node *r)
{
    r = head;
    if (r == NULL)
    {
        return;
    }
    while (r != NULL)
    {
        // printf("pid: %d, job size: %d \n", r->info.pid, r->info.bytes);
        r = r->next;
    }
    printf("\n");
}

int count()
{
    struct node *n;
    int c = 0;
    n = head;
    while (n != NULL)
    {
        n = n->next;
        c++;
    }
    return c;
}

//function to return random number in range, must of used srand()
int printRandoms(int lower, int upper,
                  int count)
{
    // srand(time(0));
    int i;
    for (i = 0; i < count; i++)
    {
        int num = (rand() %
                   (upper - lower + 1)) +
                  lower;
        return num;
    }
}

// int sample_run()
// {
//     int i, num;
//     struct node *n;
//     head = NULL;
//     while (1)
//     {
//         printf("\nList Operations\n");
//         printf("===============\n");
//         printf("1.Insert\n");
//         printf("2.Display\n");
//         printf("3.Size\n");
//         printf("4.Delete\n");
//         printf("5.Exit\n");
//         printf("Enter your choice : ");
//         if (scanf("%d", &i) <= 0)
//         {
//             printf("Enter only an Integer\n");
//             exit(0);
//         }
//         else
//         {
//             switch (i)
//             {
//             case 1:
//                 printf("Enter the number to insert : ");
//                 scanf("%d", &num);
//                 insert(num);
//                 break;
//             case 2:
//                 if (head == NULL)
//                 {
//                     printf("List is Empty\n");
//                 }
//                 else
//                 {
//                     printf("Element(s) in the list are : ");
//                 }
//                 display(n);
//                 break;
//             case 3:
//                 printf("Size of the list is %d\n", count());
//                 break;
//             case 4:
//                 if (head == NULL)
//                     printf("List is Empty\n");
//                 else
//                 {
//                     printf("Enter the number to delete : ");
//                     scanf("%d", &num);
//                     if (delete (num))
//                         printf("%d deleted successfully\n", num);
//                     else
//                         printf("%d not found in the list\n", num);
//                 }
//                 break;
//             case 5:
//                 return 0;
//             default:
//                 printf("Invalid option\n");
//             }
//         }
//     }
//     return 0;
// }

#endif