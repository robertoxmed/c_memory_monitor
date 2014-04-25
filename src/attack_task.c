#include "../include/attack_task.h"
#include "../include/papi_util.h"

void attack_list_init ( attack_list *al ){

    al->al_nb_elements = 0;
    al->al_head = NULL;
    al->al_tail = NULL;
}

void attack_list_destroy ( attack_list *al ){
    attack_element *tmp;
    tmp = al->al_head;
    while(tmp->ae_next != NULL){
        al->al_head = tmp->ae_next;
        free(tmp);
        tmp = al->al_head;
    }
    free(al);
}

void attack_element_init(attack_element *ae){
    int i,j =0;
    for(i=0;i<SIZE;i++){
        for(j=0;j<SIZE;j++){
            ae->ae_mat[i][j] = (int) (10*(float)rand()/ RAND_MAX);
        }
    }
    ae->ae_next = NULL;
    ae->ae_previous = NULL;
}

void attack_list_add_elt(attack_list *al, attack_element *ae){
    if(al == NULL || ae == NULL){
        fprintf(stderr, "Attack list or element not initialized\n");
        exit(1);
    }
    if(al->al_nb_elements == 0){ //First element added => head of the list
        al->al_head = ae;
        al->al_tail = ae;
        ae->ae_next = NULL;
        ae->ae_previous = NULL;
        al->al_index[al->al_nb_elements ++] = ae;
    }else{ //Chain elements
        al->al_tail->ae_next = ae;
        ae->ae_previous = al->al_tail;
        al->al_tail = ae;
        al->al_index[al->al_nb_elements ++] = ae;
    }
}

void attack_list_add_n_elt(attack_list *al){
    int i;
    for(i=0;i<INDEX_SIZE;i++){
        srand(i);
        attack_element *ae = (attack_element*)malloc(sizeof(attack_element));
        attack_element_init(ae);
        attack_list_add_elt(al, ae);
    }
}

void attack_element_print(attack_element *ae){
    int i, j;
    printf("-----------------\n");
    for(i=0;i<SIZE;i++){
        for(j=0;j<SIZE;j++){
            printf("%g ",ae->ae_mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");

}

void attack_list_iterate(attack_list *al){
    attack_element *iter;
    while(1){
        for(iter = al->al_head; iter; iter = iter->ae_next){
            attack_element_print(iter);
        }
    }
}

void attack_list_rand_iterate(attack_list *al){
    int i, j =0, k, cpt = 0;;
    while(1){
        srand(getpid()+i);
        i = (int)(rand()%al->al_nb_elements);
        k = (int)(rand()%100) + 1;
        printf("I = %d, K = %d\n", i, k);

        attack_element *iter = al->al_index[i];
        while((cpt != k) && (iter != NULL)){
            attack_element_print(iter);
            iter = iter->ae_next;
            cpt++;
        }
        j++;
        cpt = 0;
    }
}


int main(int argc, char **argv){

    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(atoi(argv[2]), &mask);

    if(sched_setaffinity(getpid(), sizeof(mask), &mask)){
        fprintf(stderr, "Sched error: set affinity\n");
        exit(16);
    }
    attack_list *al = (attack_list*)malloc(sizeof(attack_list));
    al->al_index = (attack_element**)malloc(INDEX_SIZE *sizeof(attack_element*));
    
    //Add the process to the table used by the hypervisor    
    attack_list_init(al);
    fprintf(stderr, "Al->nb_elts %d\n", al->al_nb_elements);
    attack_list_add_n_elt(al);
    fprintf(stderr, "Allocation done: Al->nb_elts %d\n", al->al_nb_elements);

    if(argc == 3){
        if(atoi(argv[1]) == 1){
            fprintf(stderr, "Attacker will use random iteration\n");
            attack_list_rand_iterate(al);
        }else{
            fprintf(stderr, "Attacker will use linear iteration\n");
            attack_list_iterate(al);
        }
    }else{
        fprintf(stderr, "Attacker will use linear iteration\n");
        attack_list_iterate(al);
    }

    attack_list_destroy(al);


    return EXIT_SUCCESS;
}