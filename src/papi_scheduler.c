#include "../include/papi_util.h"

#define MEMORY_QUOTA 129000
#define ITERATION_MODE "0"
#define NB_RT_ITERATION "20000000"

pid_t pid_attacker[2] = {-1, -1};
int nb_attackers = 0, send = 0;

long long rt_quota_l3 = MEMORY_QUOTA; //The quota for the cache 
int new_window = 20; // <= How many times the handler will decrement before the new window

void timer_handler(int signo, siginfo_t *info, void *context){
	int i, ret;
	//Read the values with PAPI
	if(signo == SIGRTMIN){
        scheduler_value = 0;

        if((ret = PAPI_accum(scheduler_eventset, &scheduler_value)) != PAPI_OK){
    		fprintf(stderr, "PAPI error: Couldn't read the values %s\n", PAPI_strerror(ret));
    		exit(20);
    	}
    	//Test the quota and send the sigstop
        //fprintf(stderr, "%lld\n", scheduler_value);
    	rt_quota_l3 -= scheduler_value;

    	if((rt_quota_l3 <= 0) && (send == 0)){
            fprintf(stderr, "Scheduler (%d) > Quota exceeded will stop attackers\n", getpid());
    		for(i=0; i<nb_attackers; i++)
    			kill(pid_attacker[i],SIGSTOP);
            send = 1;
    	}

    	new_window --;

    	//If new window send SIGCONT to attackers
        if(!new_window){
            fprintf(stderr, "Scheduler (%d) > New window\n", getpid());
            for(i=0; i<nb_attackers; i++)
        	   kill(pid_attacker[i], SIGCONT);
            new_window = 20;
            rt_quota_l3 = MEMORY_QUOTA;
            send = 0;
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

    scheduler_check_arguments(argc, argv);
    
    //--------------------------------PAPI initilization------------------------------//
    init_papi();
    scheduler_init_papi();
    set_option();
    scheduler_set_option();
    add_events();
    scheduler_add_event();

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
        new_tmr.it_value.tv_nsec = 250000000;
        new_tmr.it_interval.tv_sec = 0;
        new_tmr.it_interval.tv_nsec = 250000000;
    }


/**********************************************************************************************/
    //Launch the attackers
    printf("Launching %d attackers\n", atoi(argv[2]));
    for(i=0; i < atoi(argv[2]); i++){
        if((pid_attacker[nb_attackers++] = fork()) == 0){
            int stdin_fd = -1;

            CPU_ZERO(&mask);
            CPU_SET(i+2, &mask);
            stdin_fd = open("/dev/null", O_RDONLY);

            if(stdin_fd == -1)
                exit(127);
            dup2(stdin_fd, 1);
            close(stdin_fd);

            if(sched_setaffinity(getpid(), sizeof(mask), &mask)){
                fprintf(stderr, "Sched error: set affinity\n");
                exit(16);
            }
            if(execl("/usr/bin/xterm", "xterm", "-hold","-e", "./bin/attack_task", NULL) == -1){
                perror("execl");
                exit(17);
            }
            exit(0);
        }
    }

    sleep(5);
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

	    if(execl(argv[1], "RT task", NB_RT_ITERATION, NULL) == -1){
            fprintf(stderr, "Execl error: Couldn't launch RT exec\n");
            exit(17);
        }
        exit(0);
    }else if(rt_child == -1){
		fprintf(stderr, "Fork: couldn't create the RT child.\n");
		exit(16);
/**********************************************************************************************/
    //The scheduler        
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
        if((ret = PAPI_start(scheduler_eventset)) != PAPI_OK){
            fprintf(stderr, "PAPI error: failed to start counters: %s\n", PAPI_strerror(ret));
            exit(16);
        }

        struct timeval  tv1, tv2;
        gettimeofday(&tv1, NULL);

        // Wait for RT child to finish, then kill attackers
        ret = -1;
        while(ret != rt_child){ 
            ret = waitpid(rt_child, &status, 0);
        }
        if (!WIFEXITED(status)) {
            fprintf(stderr, "scheduler: Child exited with wrong status\n");
            exit(16);
        }
        for(i=0; i<nb_attackers; i++){
            fprintf(stderr, "\nScheduler (%d) > Sending signal to %d\n", getpid(), pid_attacker[i]);
            kill(pid_attacker[i], SIGKILL);
        }

        gettimeofday(&tv2, NULL);

        char time_to_char[20];
        printf ("\nTotal time = %f seconds\n",
                 (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
                 (double) (tv2.tv_sec - tv1.tv_sec));

        sprintf(time_to_char, "%f\n", 
            (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
            (double) (tv2.tv_sec - tv1.tv_sec));

        // Stopping and reading for Gnuplot
        if((ret = PAPI_stop(PAPI_EventSet, papi_values))!= PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't stop the counters %s\n", PAPI_strerror(ret));
            exit(17);
        }
        if((ret = PAPI_stop(scheduler_eventset, &scheduler_value))!= PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't stop the counters %s\n", PAPI_strerror(ret));
            exit(17);
        }

        if((ret = PAPI_read(PAPI_EventSet, papi_values)) != PAPI_OK){
            fprintf(stderr, "PAPI error : Couldn't read the values %s\n", PAPI_strerror(ret));
            exit(18);
        }

        // Printing and writting for Gnuplot
        print_counters(papi_values);
        write_miss_values(1, nb_attackers, papi_values);

        // Writting the time for Gnuplot
        int fic_time;
        char fic_name[50];

        switch(atoi(argv[2])){
            case 0:
                strcpy(fic_name, "./plot/mesures_execution_0_scheduler.data");
                break;
            case 1:
                strcpy(fic_name, "./plot/mesures_execution_1_scheduler.data");
                break;
            case 2:
                strcpy(fic_name, "./plot/mesures_execution_2_scheduler.data");
                break;
            default:
                strcpy(fic_name, "./plot/mesures_execution.data");
                break;
        }

        if ((fic_time = open(fic_name, O_RDWR | O_APPEND))==-1){
            perror("Open error on fic_time\n");
            exit(19); 
        }
        
        if (write(fic_time, time_to_char, strlen(time_to_char))==0){
            fprintf(stderr, "Write exec_time error\n");
            exit(20);
        }


        // Cleaning event sets
        if((ret=PAPI_cleanup_eventset(PAPI_EventSet))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't clean the Event Set %s\n", PAPI_strerror(ret));
            exit(19);
        }
        if((ret=PAPI_cleanup_eventset(scheduler_eventset))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't clean the Event Set %s\n", PAPI_strerror(ret));
            exit(19);
        }
        if((ret=PAPI_destroy_eventset(&PAPI_EventSet))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't destroy the Event Set %s\n", PAPI_strerror(ret));
            exit(20);
        }
        if((ret=PAPI_destroy_eventset(&scheduler_eventset))!=PAPI_OK){
            fprintf(stderr, "PAPI error: Couldn't destroy the Event Set %s\n", PAPI_strerror(ret));
            exit(20);
        }
    }

    return EXIT_SUCCESS;
}
