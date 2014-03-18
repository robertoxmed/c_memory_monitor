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



extern int nb_attackers;
extern pid_t pid_attacker[3];
extern pid_t pid_hypervisor;

extern long long papi_values[7];
extern int PAPI_EventSet;

extern void check_papi();
extern void check_arguments();
extern void print_help();
extern void print_counters(long long *values);
extern void set_option();
extern void add_events();

#endif
