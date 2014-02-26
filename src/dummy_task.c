#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

int main (int argc, char ** argv){
    int i, j;
    int core_num = sched_getcpu();

    printf("RT task executes in core number: %d\n", core_num);


    for(i=0;i<10;i++){
        for(j=0;j<=i;j++)
            printf("*");
        printf("\n");
        core_num = sched_getcpu();
        printf("RT task executes in core number: %d\n", core_num);
        sleep(1);
    }


    return 0;
}
