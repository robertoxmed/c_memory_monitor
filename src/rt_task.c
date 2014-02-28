#include "../include/rt_task.h"

void rt_task_init(rt_list *list){

    rt_element *op_1 = NULL;
    rt_element *op_2 = NULL;    
    int i,j;

    op_1 = (rt_element *)malloc(sizeof(rt_element));
    op_2 = (rt_element *)malloc(sizeof(rt_element));

    if (!op_1 || !op_2){
        printf("Erreur allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    /*Remplir deux matrices initiales*/
    for (i=0; i<SIZE; i++){
        for (j=0; j<SIZE; j++){
            if(j%2==0){
                op_2->mat[i][j] = j;
                op_1->mat[i][j] = j;
            }else {
                op_2->mat[i][j] = i;
                op_1->mat[i][j] = i;
            }
        }
    }

    list->head = op_1;
    list->op1 = op_1;
    list->op1->next = op_2;
    list->op2 = op_2;
    list->op2->next = NULL;
}

void rt_task_nextiter(rt_list *list){
    /*Calcul matriciel*/
    int i,j,k = 0;
    rt_element *res = NULL;
    
    res = (rt_element *)malloc(sizeof(rt_element));
    if (!res){
        printf("Erreur allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    /*Produit matriciel*/
    for (i=0; i<SIZE; i++){
        for (j=0; j<SIZE; j++){
            res->mat[i][j]=0; 
            for (k=0;k<SIZE;k++){ 
                res->mat[i][j] += list->op1->mat[i][k] *  list->op2->mat[k][j]; 
            }
        }
    }
    
    for (i=0; i<SIZE; i++){
        for (j=0; j<SIZE; j++){
            printf("%d ",res->mat[i][j]);
        }
        printf("\n");
    } 
    
    res->next = NULL;
    list->op2->next = res;
    list->op1 = list->op1->next;
    list->op2 = list->op2->next;
    
    printf("----------------\n");
}

void rt_task_destroy(rt_list *list){
    rt_element *p;

    p = list->head;

    while(p->next){
        list->head = p->next;
        free(p);
        p = list->head;
    }
    free(list);
}

int main(int argc, char* argv[]){
    
    rt_list *the_list = NULL;
    int i = 0;
    if(argc!= 2){
        fprintf(stderr, "Usage: %s <nb iterations>\n", argv[0]);
        exit(1);
    }

    the_list = (rt_list*)malloc(sizeof(rt_list));

    rt_task_init(the_list);

    while( i != atoi(argv[1])){
        rt_task_nextiter(the_list);
        i++;
    }
    rt_task_destroy(the_list);

    return 0;
}
