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
        printf("PAPI Wrapper Execution:\n\n");
        printf("Launching the wrapper with %s as the RT task.\n", argv[1]);
        printf("====================================\n");        
    }else{
        fprintf(stderr, "Usage: %s <RT task> <arg0> ... <argN>\n", argv[0]);
        exit(1);
    }
    
}

void check_papi(int *PAPI_EventSet){
    int retval, num_hwcntrs = 0;

    //Initialize the library
    retval = PAPI_library_init(PAPI_VER_CURRENT);

    if(retval != PAPI_VER_CURRENT){
        fprintf(stderr, "PAPI library init error!\nNow exiting.\n");
        exit(-1);
    }

    if((num_hwcntrs = PAPI_num_counters()) <= PAPI_OK){
        perror("PAPI_num_counters");
        exit(1);
    }
    if(num_hwcntrs < 2)
        fprintf(stderr, "No hardware counters or PAPI error\n");
    else
        fprintf(stderr, "This system has %d available counters.\n", num_hwcntrs);
    
    
    //Create the EventSet with existing events
    if(PAPI_create_eventset (PAPI_EventSet) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't create the Event Set.\nWill now exit.\n");
        exit(-1);
    }

    if(PAPI_query_event (PAPI_L1_DCM) != PAPI_OK){
        fprintf(stderr, "No L1 cache miss counter.\n");
        exit(-1);
    }
    if(PAPI_add_event(*PAPI_EventSet, PAPI_L1_DCM) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L1 DCM to event set\n");
        exit(-1);
    }
    if(PAPI_query_event (PAPI_L2_DCM) != PAPI_OK){
        fprintf(stderr, "No L2 cache miss counter.\n");
        //exit(-1);
    }else{
        if(PAPI_add_event(*PAPI_EventSet, PAPI_L2_DCM) != PAPI_OK){
            fprintf(stderr, "PAPI error: can't add L2 DCM to event set\n");
            exit(-1);
        }
    }
    
}

int main (int argc, char ** argv) {
    
    int PAPI_EventSet = PAPI_NULL;
    int ret;
    long_long papi_values[2];

    check_arguments(argc, argv);

    check_papi(&PAPI_EventSet);

    /*Initialize counters*/
    if((ret = PAPI_start(PAPI_EventSet)) != PAPI_OK){
        fprintf(stderr, "PAPI error: failed to start counters: %s\n", PAPI_strerror(ret));
        exit(3);
    }

    /*Child executes the RT task in one core*/
    printf("\nRT output:\n\n"); 
    if(fork() == 0){         
        cpu_set_t mask;
        /*Setting the affinity of the child*/
        CPU_ZERO(&mask);
        CPU_SET(3, &mask);
        sched_setaffinity(0, sizeof(mask), &mask);

        execl(argv[1], "RT task", NULL);
    }

    wait(NULL);

    if((ret = PAPI_read(PAPI_EventSet, papi_values)) != PAPI_OK){
        fprintf(stderr, "PAPI failed to read counters\n");
        exit(4);
    }
    
    printf("\n============================================\n");
    printf("Results:\n");
    printf("============================================\n");
    printf("L1 data cache miss %lld.\n", papi_values[0]);
    printf("L2 data cache miss %lld.\n", papi_values[1]);
    printf("============================================\n");
    printf("PAPI Wrapper End \n");
    printf("============================================\n");

    return 0;
}

void print_help(){
    printf("===========================================\n");
    printf("PAPI Wrapper Help:\n");
    printf("===========================================\n");    
    printf("Example of execution:\n");
    printf("./papi_wrapper <rt_task> <arg0> <arg1> ... <argN>\n\n");
    printf("This program will isolate a real time task in the 3rd core of the processor.\n");
    printf("The core should be isolated using [isolcpus] as Kernel command\n");
    printf("===========================================\n");

}
