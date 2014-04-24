#include "../include/papi_util.h"

long long papi_values[7];
int PAPI_EventSet = PAPI_NULL;

// Variables used by the scheduler
int scheduler_eventset = PAPI_NULL;
long long scheduler_value; //The number of memory accesses

// Variables used by the notificator
int notifier_eventset_rt = PAPI_NULL;
int notifier_eventset_a0 = PAPI_NULL;
int notifier_eventset_a1 = PAPI_NULL;
//Number of memory accesses made by the tasks
long long notifier_value_rt;
long long notifier_value_a0;
long long notifier_value_a1;

void print_help(){
    printf("===========================================\n");
    printf("PAPI Wrapper Help:\n");
    printf("===========================================\n");    
    printf("Example of execution:\n");
    printf("sudo ./papi_wrapper <rt_task> <arg0>\n\n");
    printf("This program will isolate a real time task in the 2nd core of the processor.\n");
    printf("The core should be isolated using [isolcpus] as Kernel command.\n");
    printf("===========================================\n");
}

void scheduler_print_help(){
    printf("===========================================\n");
    printf("PAPI scheduler Help:\n");
    printf("===========================================\n");    
    printf("Example of execution:\n");
    printf("sudo ./papi_scheduler <rt_task> <nb attackers>\n\n");
    printf("This program will isolate a real time task in the 2nd core of the processor.\n");
    printf("All the attackers will be in the other cores. One core should be free for the OS.\n");
    printf("The cores should be isolated using [isolcpus] as Kernel command.\n");
    printf("===========================================\n");
}

void notifier_print_help(){
    printf("===========================================\n");
    printf("PAPI Notificator Help:\n");
    printf("===========================================\n");    
    printf("Example of execution:\n");
    printf("sudo ./papi_notifier <rt_task> \n\n");
    printf("This program will isolate a real time task in the 2nd core of the processor.\n");
    printf("All the attackers will be in the other cores. One core should be free for the OS.\n");
    printf("The cores should be isolated using [isolcpus] as Kernel command.\n");
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

void scheduler_check_arguments(int argc, char **argv){
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help")) == 0){
        scheduler_print_help();
        exit(0);
    }else if(argc == 3){
        printf("============================================\n");
        printf("PAPI scheduler Execution:\n\n");
        printf("Launching the wrapper with %s as the RT task.\n", argv[1]);
        printf("============================================\n");
    }else{
        fprintf(stderr, "Usage: sudo %s <RT task> <nb attackers> \n", argv[0]);
        exit(1);
    }
}

void notifier_check_arguments(int argc, char **argv){
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help")) == 0){
        notifier_print_help();
        exit(0);
    }else if(argc == 2){
        printf("============================================\n");
        printf("PAPI Notifier Execution:\n\n");
        printf("Launching the wrapper with %s as the RT task.\n", argv[1]);
        printf("============================================\n");
    }else{
        fprintf(stderr, "Usage: sudo %s <RT task> <nb attackers> \n", argv[0]);
        exit(1);
    }
}

//------------------ Initialization of the library & creation of event sets---------------------

//Initializes the library for PAPI. Should always be called when using PAPI.
void init_papi(){
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
    
    //Create the EventSet
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

// The scheduler has one event set to count memory accesses
void scheduler_init_papi(){
    int retval;
    if((retval=PAPI_create_eventset (&scheduler_eventset)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't create the Event Set: %s.\nWill now exit.\n", PAPI_strerror(retval));
        exit(4);
    }

    if ((retval=PAPI_assign_eventset_component(scheduler_eventset, 0)) != PAPI_OK){
        fprintf(stderr, "PAPI error: component: %s\n", PAPI_strerror(retval));
        exit(5);
    }
    if((retval=PAPI_set_multiplex(scheduler_eventset)) != PAPI_OK){
        fprintf(stderr, "PAPI error: couldn't set multiplexing %s\n", PAPI_strerror(retval));
        exit(6);
    }
}

// The notifier has three event sets. One for each core/task.
void notifier_init_papi(){
    int retval;

    if((retval = PAPI_create_eventset(&notifier_eventset_rt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't create the Event Set RT: %s\nWill now exit.", PAPI_strerror(retval));
        exit(4);
    }
    if((retval = PAPI_create_eventset(&notifier_eventset_a0)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't create the Event Set A0: %s\nWill now exit.", PAPI_strerror(retval));
        exit(4);
    }
    if((retval = PAPI_create_eventset(&notifier_eventset_a1)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't create the Event Set A1: %s\nWill now exit.", PAPI_strerror(retval));
        exit(4);
    }
    if((retval = PAPI_assign_eventset_component(notifier_eventset_rt, 0)) != PAPI_OK){
        fprintf(stderr, "PAPI error: component rt: %s\n", PAPI_strerror(retval));
        exit(5);
    }
    if((retval = PAPI_assign_eventset_component(notifier_eventset_a0, 0)) != PAPI_OK){
        fprintf(stderr, "PAPI error: component a0: %s\n", PAPI_strerror(retval));
        exit(5);
    }
    if((retval = PAPI_assign_eventset_component(notifier_eventset_a1, 0)) != PAPI_OK){
        fprintf(stderr, "PAPI error: component a1: %s\n", PAPI_strerror(retval));
        exit(5);
    }
    if((retval=PAPI_set_multiplex(notifier_eventset_rt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: couldn't set multiplexing RT: %s\n", PAPI_strerror(retval));
        exit(6);
    }
    if((retval=PAPI_set_multiplex(notifier_eventset_a0)) != PAPI_OK){
        fprintf(stderr, "PAPI error: couldn't set multiplexing A0: %s\n", PAPI_strerror(retval));
        exit(6);
    }
    if((retval=PAPI_set_multiplex(notifier_eventset_a1)) != PAPI_OK){
        fprintf(stderr, "PAPI error: couldn't set multiplexing A1: %s\n", PAPI_strerror(retval));
        exit(6);
    }
}

//--------------------------- Setting the granularity of the PAPI event set. -------------------------

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

void scheduler_set_option(){
    int retval;

    //Setting the granularity for the events only the RT core is interesting
    PAPI_domain_option_t domain_opt;
    domain_opt.def_cidx = 0;
    domain_opt.eventset = scheduler_eventset;
    domain_opt.domain = PAPI_DOM_ALL;

    if((retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*)&domain_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting domain : %s\n", PAPI_strerror(retval));
        exit(7);
    }

    PAPI_granularity_option_t gran_opt;
    gran_opt.def_cidx = 0;
    gran_opt.eventset = scheduler_eventset;
    gran_opt.granularity = PAPI_GRN_SYS;

    if((retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*)&gran_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting granularity : %s\n", PAPI_strerror(retval));
        exit(8);
    }

    PAPI_cpu_option_t cpu_opt;
    cpu_opt.eventset = scheduler_eventset;
    cpu_opt.cpu_num = 1;

    if((retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*)&cpu_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't set the granularity of the events retval : %s\n", PAPI_strerror(retval));
        fprintf(stderr, "Make sure you run this program as root!\n");
        exit(9);
    }
}

void notifier_set_option(){
    int retval;

    //Setting the granularity for the events only the RT core is interesting
    PAPI_domain_option_t domain_opt;
    domain_opt.def_cidx = 0;
    domain_opt.eventset = notifier_value_rt;
    domain_opt.domain = PAPI_DOM_ALL;

    if((retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*)&domain_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting domain RT : %s\n", PAPI_strerror(retval));
        exit(7);
    }

    domain_opt.def_cidx = 0;
    domain_opt.eventset = notifier_value_a0;
    domain_opt.domain = PAPI_DOM_ALL;

    if((retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*)&domain_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting domain A0 : %s\n", PAPI_strerror(retval));
    }

    domain_opt.def_cidx = 0;
    domain_opt.eventset = notifier_value_a1;
    domain_opt.domain = PAPI_DOM_ALL;

    if((retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*)&domain_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting domain A1 : %s\n", PAPI_strerror(retval));
    }

    PAPI_granularity_option_t gran_opt;
    gran_opt.def_cidx = 0;
    gran_opt.eventset = notifier_eventset_rt;
    gran_opt.granularity = PAPI_GRN_SYS;

    if((retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*)&gran_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting granularity RT: %s\n", PAPI_strerror(retval));
        exit(8);
    }

    gran_opt.def_cidx = 0;
    gran_opt.eventset = notifier_eventset_a0;
    gran_opt.granularity = PAPI_GRN_SYS;

    if((retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*)&gran_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting granularity A0: %s\n", PAPI_strerror(retval));
        exit(8);
    }

    gran_opt.def_cidx = 0;
    gran_opt.eventset = notifier_eventset_a1;
    gran_opt.granularity = PAPI_GRN_SYS;

    if((retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*)&gran_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: Setting granularity A1: %s\n", PAPI_strerror(retval));
        exit(8);
    }

    PAPI_cpu_option_t cpu_opt;
    cpu_opt.eventset = notifier_eventset_rt;
    cpu_opt.cpu_num = 1;
    if((retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*)&cpu_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't set the granularity of the events retval : %s\n", PAPI_strerror(retval));
        exit(9);
    }

    cpu_opt.eventset = notifier_eventset_a0;
    cpu_opt.cpu_num = 2;
    if((retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*)&cpu_opt)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't set the granularity of the events retval : %s\n", PAPI_strerror(retval));
        exit(9);
    }

    cpu_opt.eventset = notifier_eventset_a1;
    cpu_opt.cpu_num = 3;
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
    }
    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_L1_ICM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L1 ICM to event set: %s.\n", PAPI_strerror(retval));
    }
    //L2 Cache miss
    if((retval=PAPI_add_event(PAPI_EventSet, PAPI_L2_DCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L2 DCM to event set: %s\n", PAPI_strerror(retval));
    }
    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_L2_ICM)) != PAPI_OK){
       fprintf(stderr, "PAPI error: can't add L2 ICM to event set: %s.\n", PAPI_strerror(retval));
    }

    //L3 Total cache miss
    if((retval=PAPI_add_event(PAPI_EventSet, PAPI_L3_TCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L3 TCM to event set: %s\n", PAPI_strerror(retval));
    }
    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_L3_TCA)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L3 TCA to event set %s\n", PAPI_strerror(retval));
    }

    if((retval = PAPI_add_event(PAPI_EventSet, PAPI_TOT_CYC)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add TOT CYC to event set %s\n", PAPI_strerror(retval));
    }
}


void scheduler_add_event(){
    int retval;

    if((retval = PAPI_add_event(scheduler_eventset, PAPI_L3_TCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L3 TCM to event set %s\n", PAPI_strerror(retval));
        fprintf(stderr, "Can't continue: scheduler needs memory access.\n");
        exit(15);
    }
}

void notifier_add_event(){
    int retval;

    if((retval = PAPI_add_event(notifier_eventset_rt, PAPI_L1_TCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L1 TCM to event set RT : %s\n", PAPI_strerror(retval));
        fprintf(stderr, "Can't continue: Notifier needs memory misses.\n");
        exit(15);
    }
    if((retval = PAPI_add_event(notifier_eventset_a0, PAPI_L1_TCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L1 TCM to event set A0:  %s\n", PAPI_strerror(retval));
        fprintf(stderr, "Can't continue: Notifier needs memory access.\n");
        exit(15);
    }
    if((retval = PAPI_add_event(notifier_eventset_a1, PAPI_L1_TCM)) != PAPI_OK){
        fprintf(stderr, "PAPI error: can't add L1 TCM to event set A1: %s\n", PAPI_strerror(retval));
        fprintf(stderr, "Can't continue: Notifier needs memory access.\n");
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
    printf("End \n");
    printf("============================================\n");
}


void write_miss_values(long long *values){

    int fic_miss;
    char values_to_char[3][40];

    sprintf(values_to_char[0],"%lld\n", values[0] + values[1]);
    sprintf(values_to_char[1],"%lld\n", values[2] + values[3]);
    sprintf(values_to_char[2],"%lld\n", values[4]);

    /*Ecriture des miss L1*/
    if (( fic_miss = open("./plot/mesures_miss_L1.data", O_RDWR | O_APPEND))==-1){
        fprintf(stderr, "Open error on fic_miss L1 data\n");
        exit(17);	
    }
    if (write(fic_miss, values_to_char[0], strlen(values_to_char[0]))==-1){
        fprintf(stderr, "Write error\n");
        exit(18);
    }

    close(fic_miss);

    /*Ecriture des miss L2*/
    if (( fic_miss = open("./plot/mesures_miss_L2.data", O_RDWR | O_APPEND))==-1){
        fprintf(stderr, "Open error on fic_miss L2 data\n");
        exit(20);	
    }

    if (write(fic_miss, values_to_char[1], strlen(values_to_char[1]))==-1){
        fprintf(stderr, "Write error\n");
        exit(21);
    }

    close(fic_miss);

    /*Ecriture des miss L3*/
    if (( fic_miss = open("./plot/mesures_miss_L3.data", O_RDWR | O_APPEND))==-1){
        fprintf(stderr, "Open error on fic_miss L3 data\n");
        exit(23);	
    }

    if (write(fic_miss, values_to_char[2], strlen(values_to_char[2]))==-1){
        fprintf(stderr, "Write error\n");
        exit(24);
    }

    close(fic_miss);
}

void write_miss_values(long long *values){

    int fic_miss;
    char values_to_char[3][40];

    sprintf(values_to_char[0],"%lld\n", values[0] + values[1]);
    sprintf(values_to_char[1],"%lld\n", values[2] + values[3]);
    sprintf(values_to_char[2],"%lld\n", values[4]);

    /*Ecriture des miss L1*/
    if (( fic_miss = open("./plot/mesures_miss_L1.data", O_RDWR | O_APPEND))==-1){
        fprintf(stderr, "Open error on fic_miss L1 data\n");
        exit(17);   
    }
    if (write(fic_miss, values_to_char[0], strlen(values_to_char[0]))==-1){
        fprintf(stderr, "Write error\n");
        exit(18);
    }

    close(fic_miss);

    /*Ecriture des miss L2*/
    if (( fic_miss = open("./plot/mesures_miss_L2.data", O_RDWR | O_APPEND))==-1){
        fprintf(stderr, "Open error on fic_miss L2 data\n");
        exit(20);   
    }

    if (write(fic_miss, values_to_char[1], strlen(values_to_char[1]))==-1){
        fprintf(stderr, "Write error\n");
        exit(21);
    }

    close(fic_miss);

    /*Ecriture des miss L3*/
    if (( fic_miss = open("./plot/mesures_miss_L3.data", O_RDWR | O_APPEND))==-1){
        fprintf(stderr, "Open error on fic_miss L3 data\n");
        exit(23);   
    }

    if (write(fic_miss, values_to_char[2], strlen(values_to_char[2]))==-1){
        fprintf(stderr, "Write error\n");
        exit(24);
    }

    close(fic_miss);
}
