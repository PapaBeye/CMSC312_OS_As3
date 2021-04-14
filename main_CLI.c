//***************************************************************************
//	Engineers:		Omar Amr, McCrae Smith, Papa Beye
//	Project:		CMSC 312 Assignment 3 - Group Project
//	Date:			04/26/2021
//***************************************************************************

#include "A3_support.c"
#include "A3_support2.c"

int main(int argc, int **argv)
{

    int p_num, t_num;
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);


    //if there are not 2 arguments in CLI
    if(argc != 3){
        printf("Sorry, you didn't input the correct number of arguments.\n");
        return 1;
    }
/////////////////////////////////////////////
    p_num = argv[1];
    t_num = argv[2];

    int i;
    for (i = 0; i < p_num; i++) // loop will run n times (n=5)
    {
        if (fork() == 0)
        {
            printf("[son] pid %d from [parent] pid %d\n", getpid(), getppid());
            exit(0);
        }
    }
    for (i = 0; i < p_num; i++) // loop will run n times (n=5)
        wait(NULL);
///////////////////////////////////////////////////////////////
    gettimeofday(&tv2, NULL);

    printf("Total time = %f seconds\n",
           (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
               (double)(tv2.tv_sec - tv1.tv_sec));

    return 0;
}