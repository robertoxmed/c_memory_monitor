#include "../include/papi_util.h"

#define NB_RT_ITERATION "20000000"

int main (int argc, char ** argv) {
  int ret;
  pid_t rt_child;
  /*long long histogram[3]; Tableau de valeurs*/

  check_arguments(argc, argv);
  //PAPI initilization
  init_papi();
  set_option();
  add_events();
  /**********************************************************************************************/
  //Child executes the RT task in one core		
  if((rt_child = fork())==0){
    int stdin_fd = -1;
    cpu_set_t mask;
    struct sched_param s_param;

    //Setting the affinity of the child
    //Using only one CPU with max priority
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);

    stdin_fd = open("/dev/null", O_RDONLY);
    if (stdin_fd == -1)
      exit(127);
    dup2(stdin_fd, 1);
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
    //The wrapper        
  }else{
    sleep(1);

    if((ret = PAPI_start(PAPI_EventSet)) != PAPI_OK){
      fprintf(stderr, "PAPI error: failed to start counters: %s\n", PAPI_strerror(ret));
      exit(16);
    }
        
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);
        
    sleep(8);

    if((ret = PAPI_stop(PAPI_EventSet, papi_values))!= PAPI_OK){
      fprintf(stderr, "PAPI error: Couldn't stop the counters %s\n", PAPI_strerror(ret));
      exit(17);
    }

    if((ret = PAPI_read(PAPI_EventSet, papi_values)) != PAPI_OK){
      fprintf(stderr, "PAPI error: Couldn't read the values %s\n", PAPI_strerror(ret));
      exit(18);
    }

    wait(NULL);
    gettimeofday(&tv2, NULL);
    
    char time_to_char[20];

    printf ("\nTotal time = %f seconds\n",
      (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec));

    sprintf(time_to_char, "%f\n",
      (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec));

    print_counters(papi_values);
        
    int fic_time;
    char fic_name[50];

    switch(atoi(argv[2])){
      case 0:
        strcpy(fic_name, "plot/mesures_execution_0_wrapper.data");
        break;
      case 1:
        strcpy(fic_name, "plot/mesures_execution_1_wrapper.data");
        break;
      case 2:
        strcpy(fic_name, "plot/mesures_execution_2_wrapper.data");
        break;
      default:
        strcpy(fic_name, "plot/mesures_execution.data");
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

    close(fic_time);

    write_miss_values(0, atoi(argv[2]), papi_values);

    if((ret=PAPI_cleanup_eventset(PAPI_EventSet))!=PAPI_OK){
      fprintf(stderr, "PAPI error: Couldn't clean the Event Set %s\n", PAPI_strerror(ret));
      exit(21);
    }

    if((ret=PAPI_destroy_eventset(&PAPI_EventSet))!=PAPI_OK){
      fprintf(stderr, "PAPI error: Couldn't destroy the Event Set %s\n", PAPI_strerror(ret));
      exit(22);
    }
  }

  return EXIT_SUCCESS;
}
