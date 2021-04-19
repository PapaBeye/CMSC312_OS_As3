#include "signal_handler.h"
#include <stdlib.h>

static void handle_sigs(int sig)
{
    /* TODO 
     *
     * 1) kill processes
     * 2) Kill threads
     * 3) deallocate use destructors EZ
     */ 
    char str[80];
    sprintf(str, "signal %d recv", sig);
    switch (sig)
    {
    case SIGHUP :
        puts(str);
        break;
    case SIGINT :
        puts(str);
        break;
    case SIGQUIT:
        puts(str);
        break;
    case SIGFPE :
        puts(str);
        break;
    case SIGKILL:
        puts(str);
        break;
    case SIGALRM:
        puts(str);
        break;
    case SIGTERM:
        puts(str);
        break;
    default:
        puts(str);
        break;
    }

}



void init_sig_hander()
{
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
}



