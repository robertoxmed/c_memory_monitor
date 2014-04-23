#include "../include/papi_util.h"

#define MEMORY_QUOTA 60000
#define THRESHOLD 500
#define ITERATION_MODE "1"
#define NB_RT_ITERATION "20000000"

pid_t pid_attacker[2] = {-1, -1};
int threshold = 0;
int nb_attackers = 2;

long long rt_quota_l1 = 0; //The quota for every task
long long a0_quota_l1 = 0;
long long a1_quota_l1 = 0;
long long current_quota;
long long total_avail_quota = MEMORY_QUOTA;

void timer_handler(int signo, siginfo_t *info, void *context){
	int i, ret;
	//Read the values with PAPI
	if(signo == SIGRTMIN){
        notifier_value_rt = 0;
        notifier_value_a0 = 0;
        notifier_value_a1 = 0;
        fprintf(stderr, "Cuurent quota = %lld\n", current_quota);

        if((ret = PAPI_accum(notifier_eventset_rt, &notifier_value_rt)) != PAPI_OK)
    		fprintf(stderr, "PAPI error: Couldn't read the RT values %s\n", PAPI_strerror(ret));
    	
        if((ret = PAPI_accum(notifier_eventset_a0, &notifier_value_a0)) != PAPI_OK)
            fprintf(stderr, "PAPI error: Couldn't read the A0 values %s\n", PAPI_strerror(ret));
            
        if((ret = PAPI_accum(notifier_eventset_a1, &notifier_value_a1)) != PAPI_OK)
            fprintf(stderr, "PAPI error: Couldn't read the A1 values %s\n", PAPI_strerror(ret));
        
    	rt_quota_l1 += notifier_value_rt;
        a0_quota_l1 += notifier_value_a0;
        a1_quota_l1 += notifier_value_a1;

        total_avail_quota -= rt_quota_l1;
        total_avail_quota -= a0_quota_l1;
        total_avail_quota -= a1_quota_l1;

        if(threshold == 0){
            // Test if a task has consumed its quota every 0.01 seconds
            if((a0_quota_l1 >= current_quota) || (a1_quota_l1 >= current_quota) || (rt_quota_l1 >= current_quota)){
                current_quota = current_quota / 3;
                fprintf(stderr, "Quota of one of the tasks exceeded. New quota = %lld\n", current_quota);
                rt_quota_l1 = 0;
                a1_quota_l1 = 0;
                a0_quota_l1 = 0;
                //If new window send SIGCONT to attackers
                if(total_avail_quota <= THRESHOLD){
                    fprintf(stderr, "There is only the THRESHOLD left. Attackers will be stopped.\n");
                    for(i=0; i<nb_attackers; i++)
                       kill(pid_attacker[i], SIGSTOP);
                    threshold ++;
                }
            }
        	
        }else if(threshold == 1){
            // In the threshold
            if(total_avail_quota <= 0){
                fprintf(stderr, "New window, the threshold was consumed.\n");
                for(i=0; i < nb_attackers; i++)
                    kill(pid_attacker[i], SIGCONT);
                threshold = 0;
                rt_quota_l1 = 0;
                a0_quota_l1 = 0;
                a1_quota_l1 = 0;
                current_quota = MEMORY_QUOTA / (nb_attackers + 1);
                total_avail_quota = MEMORY_QUOTA;
            }
        }
    }
}

int main (int argc, char ** argv) {
    int ret, status, i = 0;
	pid_t rt_child;
    timer_t tid;
    struct sigaction sa;
    struct sigevent sigev;
    struct itimerspec new_tmr;
    struct sched_param s_param;
    cpu_set_t mask;

    notifier_check_arguments(argc, argv);
    
    //--------------------------------PAPI initilization------------------------------//
    init_papi();
    notifier_init_papi();
    set_option();
    notifier_set_option();
    add_events();
    notifier_add_event();
    current_quota = MEMORY_QUOTA / (nb_attackers + 1);

    //---------------------------Timer initialization and sigaction----------------------//
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGRTMIN, &sa, NULL) == -1){
        perror("Sigaction");
        exit(2);
    }
    if(sigaction(SIGCONT, &sa, NULL) == -1){
        perror("Sigaction");
        exit(2);
    }
    
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGRTMIN;
    sigev.sigev_value.sival_ptr = &tid;
    if(timer_create(CLOCK_REALTIME, &sigev, &tid) != -1){
        new_tmr.it_value.tv_sec = 0;
        new_tmr.it_value.tv_nsec = 10000000;
        new_tmr.it_interval.tv_sec = 0;
        new_tmr.it_interval.tv_nsec = 10000000;
    }


/**********************************************************************************************/
    //Launch the attackers

    printf("Launching %d attackers\n", nb_attackers);
    for(i=0; i < nb_attackers; i++){
        if((pid_attacker[i] = fork()) == 0){
            int stdin_fd = -1;

            CPU_ZERO(&mask);
            CPU_SET(i+2, &mask);
            stdin_fd = open("/dev/null", O_RDONLY);

            if(stdin_fd == -1)
                exit(127);
            dup2(stdin_fd, 1);
            close(stdin_fd);

            if(sched_setaffinity(getpid(), sizeof(mask), &mask)){
                perror("Sched error: set affinity\n");
                exit(16);
            }
            if(execl("bin/attack_task2", "Attack task", ITERATION_MODE, NULL) == -1){
                perror("execl");
                exit(17);
            }
        }
    }

    for(i=0; i < nb_attackers; i++){ //Wait for allocation of the attackers
        pause();
    }

/**********************************************************************************************/

/**********************************************************************************************/
    //Child executes the RT task in one core	
	if((rt_child = fork())==0){
        int stdin_fd = -1;

	    //Setting the affinity of the child
        //Using only one CPU with max priority
	    CPU_ZERO(&mask);
	    CPU_SET(1, &mask);

        stdin_fd = open("/dev/null", O_RDONLY);
        if(stdin_fd == -1)
            exit(127);
        dup2(stdin_fd, 1);
        dup2(stdin_fd, 2);
        close(stdin_fd);

        s_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	    if(sched_setaffinity(getpid(), sizeof(mask), &mask)){
            fprintf(stderr, "Sched error: set affinity\n");
            exit(16);
        }
        if(sched_setscheduler(getpid(), SCHED_FIFO, &s_param)){
            fprintf(stderr, "Sched error: sched_setscheduler\n");
            exit(17);
        }

	    execl(argv[1], "RT task", NB_RT_ITERATION, NULL);
        
    }else if(rt_child == -1){
		fprintf(stderr, "Fork: couldn't create the RT child.\n");
		exit(16);
/**********************************************************************************************/
    //The Notifier        
	}else{
        sleep(1); //<= Waiting for stationnary period
        if(timer_settime(tid, 0, &new_tmr, NULL) == -1){
           perror("Timer_settime");
            exit(3);
        }

        // Launch the counters and the timer
        if((ret = PAPI_start(PAPI_EventSet)) != PAPI_OK){
            fprintf(stderr, "PAPI error: failed to start counters: %s\n", PAPI_strerror(ret));
            exit(16);
        }
        if((ret = PAPI_start(notifier_eventset_rt)) != PAPI_OK){
            fprintf(stderr, "PAPI error: failed to start counters RT: %s\n", PAPI_strerror(ret));
            exit(16);
        }
        if((ret = PAPI_start(notifier_eventset_a0)) != PAPI_OK){
            fprintf(stderr, "PAPI error: failed to start counters A0: %s\n", PAPI_strerror(ret));
            exit(16);
        }
        if((ret = PAPI_start(notifier_eventset_a1)) != PAPI_OK){
            fprintf(stderr, "PAPI error: failed to start counters A1: %s\n", PAPI_strerror(ret));
            exit(16);
        }

        struct timeval  tv1, tv2;
        gettimeofday(&tv1, NULL);

        ret = -1;
        while(ret != rt_child){ //Wait for RT child to finish
            ret = waitpid(rt_child, &status, 0);
        }
        if (!WIFEXITED(status)) {
            fprintf(stderr, "Hypervisor: Child exited with wrong status\n");
            exit(16);
        }

        gettimeofday(&tv2, NULL);

        printf ("\nTotal time = %f seconds\n",
                 (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
                 (double) (tv2.tv_sec - tv1.tv_sec));

        if((ret = PAPI_stop(PAPI_EventSet, papi_values))!= PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't stop the counters %s\n", PAPI_strerror(ret));
            exit(17);
        }
        if((ret = PAPI_stop(notifier_eventset_rt, &notifier_value_rt))!= PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't stop the counters %s\n", PAPI_strerror(ret));
            exit(17);
        }
        if((ret = PAPI_stop(notifier_eventset_a0, &notifier_value_a0))!= PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't stop the counters %s\n", PAPI_strerror(ret));
            exit(17);
        }
        if((ret = PAPI_stop(notifier_eventset_a1, &notifier_value_a1))!= PAPI_OK){
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
        if((ret=PAPI_cleanup_eventset(notifier_eventset_rt))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't clean the Event Set %s\n", PAPI_strerror(ret));
            exit(19);
        }
        if((ret=PAPI_destroy_eventset(&notifier_eventset_rt))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't destroy the Event Set %s\n", PAPI_strerror(ret));
            exit(20);
        }
        if((ret=PAPI_cleanup_eventset(notifier_eventset_a0))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't clean the Event Set %s\n", PAPI_strerror(ret));
            exit(19);
        }
        if((ret=PAPI_destroy_eventset(&notifier_eventset_a0))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't destroy the Event Set %s\n", PAPI_strerror(ret));
            exit(20);
        }
        if((ret=PAPI_cleanup_eventset(notifier_eventset_a1))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't clean the Event Set %s\n", PAPI_strerror(ret));
            exit(19);
        }
        if((ret=PAPI_destroy_eventset(&notifier_eventset_a1))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't destroy the Event Set %s\n", PAPI_strerror(ret));
            exit(20);
        }

    }
    for(i=0; i<nb_attackers; i++){
        kill(pid_attacker[i], SIGINT);
    }
    
    return EXIT_SUCCESS;
}
