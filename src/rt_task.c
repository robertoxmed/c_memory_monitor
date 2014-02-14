#include "../include/rt_task.h"

void init_timer(rt_task * task, int delay){
    struct sigevent sig_ev;
    strunct itimerspec new_t, old_t;
    sig_ev.sigev_notify = SIGEV_SIGNAL;
    sig_ev.sigev_signo = SIGTRMIN;
    sig_ev.sigev_value.sival_ptr = task.rt_timer;
    
    if(timer_creat(CLOCK_REALTIME, &sig_ev, &task.timer) != -1){
        new_t.it_value.tv_sec = 0;
        new_t.it_value.tv_usec = delay;
        new_t.it_interval.tv_sec = 0;
        new_t.it_interval.tv_usec = delay;
    }else{
        fprintf(stderr, "Timer non initialisé\n");
        exit(1);
    }
    timer_settime(task.rt_timer, 0, &new_t, &old_t);
}

void signal_handler(int signo, siginfo_t *info, void *context){
    if(signo == SIGTRMIN){
        //do work
        //Do a write in L1 cache
    }
}

int main (int argc, char ** argv){

    rt_task the_task;
    memset(&the_task, 0, sizeof(the_task));
    
    if((the_task.fd_w = open("../demo/example", O_RDWR)) == -1){
        perror("open");
        exit(1);
    }

    if(argc != 2){
        fprintf(stderr, "Paramètres par défaut utilisés\n");
        fprintf(stderr, "Timer à 100 ms\n";
        init_timer(&the_task, 100);

    }else{
        fprintf(stderr, "Timer à %d\n", atoi(argv[1]));
        init_timer(&the_task, atoi(argv[1]);
    }

    for(;;) ; //Idle until timer launches

    return 0;
}
