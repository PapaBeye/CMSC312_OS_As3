#include "signal_handler.h"
#include <stdlib.h>
#include <unistd.h>

static void handle_sigs(int sig)
{
    shmctl(_sigdata->shmID, IPC_RMID, NULL);
    char str[80];
    sprintf(str, "signal %d recv\n", sig);
    puts(str);
    int i =0 ;
    while(i < (_sigdata->_numPr -1)){
        sprintf(str, "Killing child process %d\n", _sigdata->prcs[i]);
        puts(str);
        if (kill((pid_t) _sigdata->prcs[i], SIGINT)== -1){
            fputs("An error occurred while killing a child process, Sending kill sig again \n", stderr);
            kill((pid_t) _sigdata->prcs[i], SIGINT);
        }
        waitpid(_sigdata->prcs[i], NULL, 1);
        i++;
    }
    i = 0;
    sprintf(str, "Killing threads\n");
    puts(str);
    while(i < _sigdata->_numTh){
        if (pthread_kill(_sigdata->thrds[i], 9) != 0){
            fputs("An error occurred while Canceling thread\n", stderr);
        }
        i++;
    }
    puts("Killing Self");
    kill(getpid(), 9);
}



void init_sig_hander(sig_dt* sigdata)
{
    _sigdata = sigdata;
    if (signal(SIGHUP, handle_sigs)  == SIG_ERR) {
        fputs("An error occurred while setting SIGHUP handler.\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, handle_sigs) == SIG_ERR) {
        fputs("An error occurred while setting SIGINT handler.\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, handle_sigs) == SIG_ERR) {
        fputs("An error occurred while setting SIGQUIT handler.\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (signal(SIGFPE, handle_sigs) == SIG_ERR) {
        fputs("An error occurred while setting SIGFPE handler.\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (signal(SIGALRM, handle_sigs) == SIG_ERR) {
        fputs("An error occurred while setting SIGALRM handler.\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTERM, handle_sigs) == SIG_ERR) {
        fputs("An error occurred while setting SIGTERM handler.\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (signal(SIGSEGV, handle_sigs) == SIG_ERR) {
        fputs("An error occurred while setting SIGSEGV handler.\n", stderr);
        exit(EXIT_FAILURE);
    }
}

