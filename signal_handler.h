
#include <stdio.h>
#include <signal.h>
#include <time.h>


pid_t *prcs;
void init_sig_hander();

static void handle_sigs(int sig);

