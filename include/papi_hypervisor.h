#ifndef __PAPI_HYPERVISOR_
#define __PAPI_HYPERVISOR_

#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/wait.h>

extern pid_t pid_attacker1;
extern pid_t pid_attacker2;
extern pid_t pid_attacker3;

extern void check_papi();
extern void check_arguments();
extern void print_help();
extern void print_counters(long long *values);
extern void set_option();
extern void add_events();

#endif
