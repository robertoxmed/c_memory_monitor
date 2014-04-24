#ifndef __PAPI_HYPERVISOR_
#define __PAPI_HYPERVISOR_

#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int scheduler_eventset;
extern long long scheduler_value; //The number of memory accesses

extern long long papi_values[7];
extern int PAPI_EventSet;

extern int notifier_eventset_rt;
extern int notifier_eventset_a0;
extern int notifier_eventset_a1;
extern long long notifier_value_rt;
extern long long notifier_value_a0;
extern long long notifier_value_a1;

void init_papi();

// Used by PAPI Wrapper
void check_arguments(int argc, char **argv);
void print_help();
void print_counters(long long *values);
void set_option();
void add_events();
void write_miss_values(long long *values);
 
// Used by PAPI Scheduler
void scheduler_check_arguments(int argc, char **argv);
void scheduler_print_help();
void scheduler_init_papi();
void scheduler_set_option();
void scheduler_add_event();

// Used by PAPI Notifier
void notifier_check_arguments(int argc, char **argv);
void notifier_print_help();
void notifier_init_papi();
void notifier_set_option();
void notifier_add_event();

#endif
