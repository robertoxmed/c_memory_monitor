#include <stdlib.h>
#include <stdio.h>

#define SIZE 3

typedef struct op{
  int mat[SIZE][SIZE];
  struct op *next;
}list_op;

/*Paramètre ite = nombre de produits matriciels à effectuer */
int rt_task(int ite){

  list_op *head = NULL;
  list_op *op_1 = NULL;
  list_op *op_2 = NULL;
  list_op *res = NULL;
    
  int i,j;

  op_1 = (list_op *)malloc(sizeof(list_op));
  op_2 = (list_op *)malloc(sizeof(list_op));

  if (!op_1 || !op_2){
    printf("Erreur allocation mémoire\n");
    exit(EXIT_FAILURE);
  }

  head = op_1;
  op_1->next = op_2;

  /*Remplir deux matrices initiales*/
  for (i=0; i<SIZE; i++){
    for (j=0; j<SIZE; j++){
      if(j%2==0){
	op_2->mat[i][j] = j;
	head->mat[i][j] = j;
      }
      else {
	op_2->mat[i][j] = i;
	head->mat[i][j] = i;
      }
    }
  }


  /*Calcul matriciel*/
  int k,l = 0;
  
  printf("Début de calcul\n");
  printf("----------------\n");
  while(l!=ite){
    res = (list_op *)malloc(sizeof(list_op));
    op_2->next=res;

    if (!res){
      printf("Erreur allocation mémoire\n");
      exit(EXIT_FAILURE);
    }
  
    /*Produit matriciel*/
    for (i=0; i<SIZE; i++){
      for (j=0; j<SIZE; j++){
	res->mat[i][j]=0; 
	for (k=0;k<SIZE;k++) 
	  { 
	    res->mat[i][j] += op_1->mat[i][k] *  op_2->mat[k][j]; 
	  }
      }
    }
     for (i=0; i<SIZE; i++){
       for (j=0; j<SIZE; j++){
	 printf("%d ",res->mat[i][j]);
       }
       printf("\n");
     } 
     op_1 = op_1->next;
     op_2 = op_2->next;
     res= res->next;

     l++;
     printf("----------------\n");
  }



  /*Libération mémoire*/

  while(head->next){
    res=head;
    head = head->next;
    free(res);
    res=NULL;
  }

  free(head);

  return 0;
}



int main(int argc, char* argv[]){

  rt_task(atoi(argv[1]));

  return 0;

}
