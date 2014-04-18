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

extern int hypervisor_eventset;
extern long long hypervisor_value; //The number of memory accesses

extern long long papi_values[7];
extern int PAPI_EventSet;

extern int notifier_eventset_rt;
extern int notifier_eventset_a0;
extern int notifier_eventset_a1;
extern long long notifier_value_rt;
extern long long notifier_value_a0;
extern long long notifier_value_a1;

// Used by PAPI Wrapper
void check_papi();
void check_arguments(int argc, char **argv);
void print_help();
void print_counters(long long *values);
void set_option();
void add_events();
void write_miss_values(long long *values);
 
// Used by PAPI Hypervisor
void check_hypervisor_arguments(int argc, char **argv);
void print_hypervisor_help();
void init_papi_hypervisor();
void set_option_hypervisor();
void add_event_hypervisor();

// Used by PAPI Notifier
void check_notifier_arguments(int argc, char **argv);
void print_notifier_help();
void init_papi_notifier();
void set_option_notifier();
void add_event_notifier();

#endif
