#include <stdlib.h>
#include <stdio.h>

int attaquant(int tempo){

  int ite=0;
  FILE *fic = NULL;

  while(ite != tempo){

    if(!(fic = fopen("attaquant.txt", "w"))){
      printf("error fopen\n");
      return(1);
    }

    if(fprintf(fic, "%d", ite)<=0){
      printf("error fprintf\n");
      return(1);
    }

    printf("valeur de ite %d \n",ite);

    fclose(fic);

    ite++;
  }

  return 0;
}


int main(int argc, char* argv[]){

  int res;

  if (argc != 2){
    printf("error number of arguments\n");
    return(EXIT_FAILURE);
  }

  res = attaquant(atoi(argv[1]));

  if (res){
    printf("error during the attack\n");
    return(EXIT_FAILURE);
  }  


  return 0;
}
