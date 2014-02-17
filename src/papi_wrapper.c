//gcc -I/usr/local/include -O0 src/papi_wrapper.c /usr/local/lib/libpapi.a -Wall -g  -o bin/papi_wrapper


#define _GNU_SOURCE

#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

int main (int argc, char ** argv) {
    
    int papi_events[2] = { PAPI_L1_DCM, PAPI_L2_DCM };
    int ret, num_hwcntrs = 0;
    long long values[2];

    if(argc != 1){
        fprintf(stderr, "Usage: %s <RT task>\n", argv[0]);
        exit(1);
    }
    
    if((num_hwcntrs = PAPI_num_counters()) <= PAPI_OK){
        perror("PAPI_num_counters");
        exit(1);
    }
    if(num_hwcntrs < 2)
        fprintf(stderr, "No hardware counters or PAPI error\n");
    else
        fprintf(stderr, "This system has %d available counters.\n", num_hwcntrs);

    /*Initialize counters*/
    if((ret = PAPI_start_counters(papi_events, 2)) != PAPI_OK){
        fprintf(stderr, "PAPI failed to start counters: %s\n", PAPI_strerror(ret));
        exit(3);
    }
    
    if(fork() == 0){ /*Child executes the RT task in one core*/
        int i;
        cpu_set_t mask;
        /*Setting the affinity of the child*/
        CPU_ZERO(&mask);
        CPU_SET(1, &mask);
        sched_setaffinity(0, sizeof(mask), &mask);

        for(i=0;i<10;i++){
            printf("%d> I'm the child process\n", getpid());
            sleep(1);
        }
        
        exit(0);
    }
    wait(NULL);

    if((ret = PAPI_read_counters(values, 2)) != PAPI_OK){
        fprintf(stderr, "PAPI failed to read counters\n");
        exit(4);
    }
    
    fprintf(stdout, "L1 data cache miss %lld.\n", values[0]);
    fprintf(stdout, "L2 data cache miss %lld.\n", values[1]);

    return 0;
}
