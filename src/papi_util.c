#include "../include/papi_util.h"

long long papi_values[7];
int PAPI_EventSet = PAPI_NULL;

int hypervisor_eventset = PAPI_NULL;
long long hypervisor_value; //The number of memory accesses

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

void print_hypervisor_help(){
    printf("===========================================\n");
    printf("PAPI Hypervisor Help:\n");
    printf("===========================================\n");    
    printf("Example of execution:\n");
    printf("sudo ./papi_wrapper <rt_task> <nb attackers>\n\n");
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

void check_hypervisor_arguments(int argc, char **argv){
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help")) == 0){
        print_hypervisor_help();
        exit(0);
    }else if(argc == 3){
        printf("============================================\n");
        printf("PAPI Hypervisor Execution:\n\n");
        printf("Launching the wrapper with %s as the RT task.\n", argv[1]);
        printf("============================================\n");
    }else{
        fprintf(stderr, "Usage: sudo %s <RT task> <nb attackers> \n", argv[0]);
        exit(1);
    }
}

//Checks the available events in the host's processor
void check_papi(){
    int retval;

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

void init_papi_hypervisor(){
    int retval;
    //Create the EventSet with existing events
    if((retval=PAPI_create_eventset (&hypervisor_eventset)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't create the Event Set: %s.\nWill now exit.\n", PAPI_strerror(retval));
        exit(4);
    }

    if ((retval=PAPI_assign_eventset_component(hypervisor_eventset, 0)) != PAPI_OK){
        fprintf(stderr, "PAPI error: component: %s\n", PAPI_strerror(retval));
        exit(5);
    }
    if((retval=PAPI_set_multiplex(hypervisor_eventset)) != PAPI_OK){
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

void set_option_hypervisor(){
    int retval;

    //Setting the granularity for the events only the RT core is interesting
    PAPI_domain_option_t domain_opt;
    domain_opt.def_cidx = 0;
    domain_opt.eventset = hypervisor_eventset;
    domain_opt.domain = PAPI_DOM_ALL;

    if((retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*)&domain_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting domain : %s\n", PAPI_strerror(retval));
        exit(7);
    }

    PAPI_granularity_option_t gran_opt;
    gran_opt.def_cidx = 0;
    gran_opt.eventset = hypervisor_eventset;
    gran_opt.granularity = PAPI_GRN_SYS;

    if((retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*)&gran_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting granularity : %s\n", PAPI_strerror(retval));
        exit(8);
    }

    PAPI_cpu_option_t cpu_opt;
    cpu_opt.eventset = hypervisor_eventset;
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


void add_event_hypervisor(){
    int retval;

    if((retval = PAPI_add_event(hypervisor_eventset, PAPI_L3_TCA)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L3 TCA to event set %s\n", PAPI_strerror(retval));
        exit(15);
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

void print_counters_hypervisor(long long *values){

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
    printf("PAPI Hypervisor End \n");
    printf("============================================\n");
}


void write_miss_values(long long *values){

  int fic_miss;
 
  /*Ecriture des miss L1*/
 if (( fic_miss = open("doc/mesures_miss_L1.data", O_RDWR | O_APPEND | O_CREAT))==-1){
    fprintf(stderr, "Open error on fic_miss\n");
    exit(17);	
 }

 if (write(fic_miss, &values[0], sizeof(long long))==-1){
   fprintf(stderr, "Write error\n");
   exit(18);
 }

 if (write(fic_miss, &values[1], sizeof(long long))==-1){
   fprintf(stderr, "Write error\n");
   exit(19);
 }

 /*Ecriture des miss L2*/
 if (( fic_miss = open("doc/mesures_miss_L2.data", O_RDWR | O_APPEND | O_CREAT))==-1){
   fprintf(stderr, "Open error on fic_miss\n");
   exit(20);	
 }

 if (write(fic_miss, &values[2], sizeof(long long))==-1){
   fprintf(stderr, "Write error\n");
   exit(21);
 }

 if (write(fic_miss, &values[3], sizeof(long long))==-1){
   fprintf(stderr, "Write error\n");
   exit(22);
 }

 /*Ecriture des miss L3*/
 if (( fic_miss = open("doc/mesures_miss_L3.data", O_RDWR | O_APPEND | O_CREAT))==-1){
   fprintf(stderr, "Open error on fic_miss\n");
   exit(23);	
 }

 if (write(fic_miss, &values[4], sizeof(long long))==-1){
   fprintf(stderr, "Write error\n");
   exit(24);
 }

 if (write(fic_miss, &values[5], sizeof(long long))==-1){
   fprintf(stderr, "Write error\n");
   exit(25);
 }

 close(fic_miss);
}
