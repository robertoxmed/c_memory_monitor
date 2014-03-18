#include "../include/rt_task2.h"

void rt_task_init(int tab_1[SIZE], int tab_2[SIZE]){

  int i=0;
  
  srand(time(NULL));
  for (i=0; i<SIZE; i++){
    tab_1[i] =  (int) (10*(float)rand()/ RAND_MAX);
    tab_2[i] =  (int) (10*(float)rand()/ RAND_MAX);
  }
}

void rt_task_treatement(int tab_1[SIZE],int tab_2[SIZE], int i){

  int decision, tab_rand;
  
  srand(i);
  decision = rand();
  tab_rand = rand()%SIZE;
 
  printf("--------------------\n");
  printf("valeur de decision %d \n", decision);
    
  if(decision < (RAND_MAX/2)){
    printf("valeur lue dans tab_1[%d]: %d\n", tab_rand, tab_1[tab_rand]);
  }
  
  else{
    
    printf("valeur lue dans tab_2[%d]: %d\n", tab_rand, tab_2[tab_rand]);
  }
}
 

int main(int argc, char* argv[]){
  
  if (argc!=2){
    fprintf(stderr, "Usage: %s <nb iterations>\n", argv[0]);
    exit(1); 
  }

  int tab_1[SIZE];
  int tab_2[SIZE];
  int nb_iter = atoi(argv[1]), i=0;

  rt_task_init(tab_1, tab_2);

  while (i!=nb_iter){
    rt_task_treatement(tab_1,tab_2,i);
    i++;
    }
 
  return 0;

}
