#include "../include/papi_util.h"

long long papi_values[7];
int PAPI_EventSet = PAPI_NULL;
pid_t pid_hypervisor = -1;
pid_t pid_attacker[3] = {-1, -1, -1};
int nb_attackers = 0;

void print_help(){
    printf("===========================================\n");
    printf("PAPI Wrapper Help:\n");
    printf("===========================================\n");    
    printf("Example of execution:\n");
    printf("sudo ./papi_wrapper <rt_task> <arg0>\n\n");
    printf("This program will isolate a real time task in the 2nd core of the processor.\n");
    printf("The core should be isolated using [isolcpus] as Kernel command\n");
    printf("===========================================\n");
}

void check_arguments (int argc, char ** argv){
    if(argc == 2 && (strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0)){
        print_help();
        exit(0);
    }else if (argc == 3){
        printf("============================================\n");
        printf("PAPI Wrapper Execution:\n\n");
        printf("Launching the wrapper with %s as the RT task.\n", argv[1]);
        printf("============================================\n");
    }else{
        fprintf(stderr, "Usage: sudo %s <RT task> <arg0> \n", argv[0]);
        exit(1);
    }   
}

//Checks the available events in the host's processor
void check_papi(){
    int retval, num_hwcntrs = 0;

    //Initialize the library
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if(retval != PAPI_VER_CURRENT){
        fprintf(stderr, "PAPI error: library init error! %s\nNow exiting.\n", PAPI_strerror(retval));
        exit(1);
    }

    if((retval=PAPI_multiplex_init())!=PAPI_OK){
        fprintf(stderr, "PAPI error: can't Initialize multiplexing %s\n", PAPI_strerror(retval));
        exit(2);
    }

    if((num_hwcntrs = PAPI_num_counters()) <= PAPI_OK){
        perror("PAPI_num_counters");
        exit(3);
    }
    if(num_hwcntrs < 2)
        fprintf(stderr, "No hardware counters or PAPI error\n");
    else
        fprintf(stderr, "This system has %d available counters.\n", num_hwcntrs);
    
    //Create the EventSet with existing events
    if((retval=PAPI_create_eventset (&PAPI_EventSet)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't create the Event Set: %s.\nWill now exit.\n", PAPI_strerror(retval));
        exit(4);
    }

    if ((retval=PAPI_assign_eventset_component(PAPI_EventSet, 0)) != PAPI_OK){
        fprintf(stderr, "PAPI error: component: %s\n", PAPI_strerror(retval));
        exit(5);
    }

    if((retval=PAPI_set_multiplex(PAPI_EventSet)) != PAPI_OK){
        fprintf(stderr, "PAPI error: couldn't set multiplexing %s\n", PAPI_strerror(retval));
        exit(6);
    }
}

//Setting the granularity of the PAPI event set.
//The set will be attached to the RT child. Couldn't attach to the core itself.
void set_option(){
	int retval;

    //Setting the granularity for the events only the RT core is interesting
    PAPI_domain_option_t domain_opt;
    domain_opt.def_cidx = 0;
    domain_opt.eventset = PAPI_EventSet;
    domain_opt.domain = PAPI_DOM_ALL;

    if((retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*)&domain_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting domain : %s\n", PAPI_strerror(retval));
        exit(7);
    }

    PAPI_granularity_option_t gran_opt;
    gran_opt.def_cidx = 0;
    gran_opt.eventset = PAPI_EventSet;
    gran_opt.granularity = PAPI_GRN_SYS;

    if((retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*)&gran_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting granularity : %s\n", PAPI_strerror(retval));
        exit(8);
    }

	PAPI_cpu_option_t cpu_opt;
    cpu_opt.eventset = PAPI_EventSet;
    cpu_opt.cpu_num = 1;

	if((retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*)&cpu_opt)) != PAPI_OK){
		fprintf(stderr, "PAPI error: can't set the granularity of the events retval : %s\n", PAPI_strerror(retval));
        fprintf(stderr, "Make sure you run this program as root!\n");
		exit(9);
	}
}

void add_events(){
    int retval;
    //L1 Cache miss
    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_L1_DCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L1 DCM to event set: %s.\n", PAPI_strerror(retval));
        exit(10);
    }
    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_L1_ICM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L1 ICM to event set: %s.\n", PAPI_strerror(retval));
        exit(11);
    }
    //L2 Cache miss
    if((retval=PAPI_add_event(PAPI_EventSet, PAPI_L2_DCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L2 DCM to event set: %s\n", PAPI_strerror(retval));
        exit(12);
    }
    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_L2_ICM)) != PAPI_OK){
       fprintf(stderr, "PAPI error: can't add L2 ICM to event set: %s.\n", PAPI_strerror(retval));
        exit(13);
    }

    //L3 Total cache miss
    if((retval=PAPI_add_event(PAPI_EventSet, PAPI_L3_TCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L3 TCM to event set: %s\n", PAPI_strerror(retval));
        exit(14);
    }
    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_L3_TCA)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L3 TCA to event set %s\n", PAPI_strerror(retval));
        exit(15);
    }

    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_TOT_CYC)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add TOT CYC to event set %s\n", PAPI_strerror(retval));
        exit(16);
    }
}

void print_counters(long long *values){

    printf("\n============================================\n");
    printf("Results:\n");
    printf("============================================\n");
    printf("L1 data cache miss %lld.\n", values[0]);
    printf("L1 instruction cache miss %lld.\n\n", values[1]);
    printf("L2 data cache miss %lld.\n", values[2]);
    printf("L2 instruction cache miss %lld.\n", values[3]);
    printf("=> L2 cache data hit rate %2.3f\n\n", (1.0 - ((double)values[2]/(double)values[0])) * 100);
    printf("L3 total cache miss %lld.\n", values[4]);
    printf("L3 total cache access %lld.\n", values[5]);
    printf("=> L3 cache hit rate %2.3f\n\n", ((double)values[4]/(double)values[5]) * 100);
    printf("Total cycles %lld\n", values[6]);
    printf("============================================\n");
    printf("PAPI Wrapper End \n");
    printf("============================================\n");
}
