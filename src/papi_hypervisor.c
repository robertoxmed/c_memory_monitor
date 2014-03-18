#include "../include/papi_util.h"

#define CACHE_QUOTA 500000

//The quota for the cache 
long long rt_quota_l1 = CACHE_QUOTA;
long long at_quota_l1 = CACHE_QUOTA;
int new_window = 5; // <= How many times the handler will decrement before the new window

void timer_handler(int signo, siginfo_t *info, void *context){
    int i, ret;
    if(signo == SIGRTMIN){ //Signal send by the timer => Check quota
        //Read the values with PAPI
        if((ret = PAPI_read(PAPI_EventSet, papi_values)) != PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't read the values %s\n", PAPI_strerror(ret));
            exit(20);
        }
        //Test the quota and send the sigstop
        rt_quota_l1 -= papi_values[0];
        if(rt_quota_l1 <= 0){
            for(i=0; i<nb_attackers; i++)
                kill(pid_attacker[i],SIGSTOP);
        }

        new_window --;
        //If new window send SIGCONT to attackers
        /*
        if(!new_window){
            for(i=0; i<nb_attackers; i++)
                kill(pid_attacker[i], SIGCONT);
            new_window = 5;
            rt_quota_l1 = CACHE_QUOTA;
            at_quota_l1 = CACHE_QUOTA;
        }
        */
    }
}

int main (int argc, char ** argv) {
    int ret;
	pid_t rt_child;
    timer_t tid;
    struct sigaction sa;
    struct sigevent sigev;
    struct itimerspec new_tmr;

    check_arguments(argc, argv);
    //PAPI initilization
    check_papi();
    set_option();
    add_events();

    pid_hypervisor = getpid();

    //Timer initialization and sigaction
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGRTMIN, &sa, NULL) == -1){
        perror("Sigaction");
        exit(2);
    }
    
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGRTMIN;
    sigev.sigev_value.sival_ptr = &tid;
    if(timer_create(CLOCK_REALTIME, &sigev, &tid) != -1){
        new_tmr.it_value.tv_sec = 0;
        new_tmr.it_value.tv_nsec = 50;
        new_tmr.it_interval.tv_sec = 0;
        new_tmr.it_interval.tv_nsec = 50;
    }
    if(timer_settime(tid, 0, &new_tmr, NULL) == -1){
        perror("Timer_settime");
        exit(3);
    }
/**********************************************************************************************/
    //Child executes the RT task in one core		
	if((rt_child = fork())==0){

		cpu_set_t mask;
        struct sched_param s_param;

	    //Setting the affinity of the child
        //Using only one CPU with max priority
	    CPU_ZERO(&mask);
	    CPU_SET(1, &mask);

        s_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	    if(sched_setaffinity(getpid(), sizeof(mask), &mask)){
            fprintf(stderr, "Sched error: set affinity\n");
            exit(16);
        }
        if(sched_setscheduler(getpid(), SCHED_FIFO, &s_param)){
            fprintf(stderr, "Sched error: sched_setscheduler\n");
            exit(17);
        }

	    execl(argv[1], "RT task", argv[2], NULL);
        
    }else if(rt_child == -1){
		fprintf(stderr, "Fork: couldn't create the RT child.\n");
		exit(16);
/**********************************************************************************************/
    //The wrapper        
	}else{

        if((ret = PAPI_start(PAPI_EventSet)) != PAPI_OK){
            fprintf(stderr, "PAPI error: failed to start counters: %s\n", PAPI_strerror(ret));
            exit(16);
        }
        
        struct timeval  tv1, tv2;
        gettimeofday(&tv1, NULL);
        wait(NULL);
        gettimeofday(&tv2, NULL);

        printf ("\nTotal time = %f seconds\n",
                 (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
                 (double) (tv2.tv_sec - tv1.tv_sec));

        if((ret = PAPI_stop(PAPI_EventSet, papi_values))!= PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't stop the counters %s\n", PAPI_strerror(ret));
            exit(17);
        }

        if((ret = PAPI_read(PAPI_EventSet, papi_values)) != PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't read the values %s\n", PAPI_strerror(ret));
            exit(18);
        }

        print_counters(papi_values);
        
        if((ret=PAPI_cleanup_eventset(PAPI_EventSet))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't clean the Event Set %s\n", PAPI_strerror(ret));
            exit(19);
        }

        if((ret=PAPI_destroy_eventset(&PAPI_EventSet))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't destroy the Event Set %s\n", PAPI_strerror(ret));
            exit(20);
        }

    }

    return EXIT_SUCCESS;
}
