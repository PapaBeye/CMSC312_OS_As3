

/* Papa Beye
 CMSC 312 SPRING 21
 4/23/2021 
 Professor Ghosh
 */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>


struct sigdata
{
    int *prcs;
    int *thrdsId;
    pthread_t *thrds;
    int _numTh;
    int _numPr;
    int shmID;
};

typedef struct sigdata sig_dt;


sig_dt* _sigdata;

void init_sig_hander(sig_dt* sigdata);

static void handle_sigs(int sig);

