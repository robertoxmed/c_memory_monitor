//gcc -I/usr/local/include -O0 src/papi_wrapper.c /usr/local/lib/libpapi.a -Wall -g  -o bin/papi_wrapper


#define _GNU_SOURCE

#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

void print_help();

void check_arguments (int argc, char ** argv){
    if(argc == 2 && (strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0)){
        print_help();
        exit(0);
    }else if (argc >= 2){
        printf("====================================\n");
        printf("papi_wrapper execution:\n");
        printf("==================================\n");
        printf("Launching the wrapper with %s as the RT task.\n", argv[1]);
    }else{
        fprintf(stderr, "Usage: %s <RT task> <arg0> ... <argN>\n", argv[0]);
        exit(1);
    }
    
}

void check_papi(){
    int retval;

    //Initialize the library
    retval = PAPI_library_init(PAPI_VER_CURRENT);

    if(retval != PAPI_VER_CURRENT){
        fprintf(stderr, "PAPI library init error!\nNow exiting.\n");
        exit(-1);
    }

    //Check if the events exist
    if(PAPI_query_event (PAPI_L1_DCM) != PAPI_OK){
        fprintf(stderr, "No L1 cache miss counter.\n");
        exit(-1);
    }
    if(PAPI_query_event (PAPI_L2_DCM) != PAPI_OK){
        fprintf(stderr, "No L2 cache miss counter.\n");
        exit(-1);
    }
    
}

int main (int argc, char ** argv) {
    
    int papi_events[2] = { PAPI_L1_DCM, PAPI_L2_DCM };
    int ret, num_hwcntrs = 0;
    long_long papi_values[2];

    check_arguments(argc, argv);

    check_papi();

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

    /*Child executes the RT task in one core*/
    
    if(fork() == 0){         
        cpu_set_t mask;
        /*Setting the affinity of the child*/
        CPU_ZERO(&mask);
        CPU_SET(3, &mask);
        sched_setaffinity(0, sizeof(mask), &mask);

        execl(argv[1], "RT task", NULL);
    }

    wait(NULL);

    if((ret = PAPI_read_counters(papi_values, 2)) != PAPI_OK){
        fprintf(stderr, "PAPI failed to read counters\n");
        exit(4);
    }
    
    printf("L1 data cache miss %lld.\n", papi_values[0]);
    printf("L2 data cache miss %lld.\n", papi_values[1]);
    printf("================================================\n");
    printf("end of papi_wrapper\n");

    return 0;
}

void print_help(){
    printf("papi_wrapper help:\n===========================================\n");
    printf("Example of execution:\n");
    printf("./papi_wrapper <rt_task> <arg0> <arg1> ... <argN>\n\n");
    printf("===========================================\n");

}
