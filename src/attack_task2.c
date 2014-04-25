#include "../include/attack_task.h"
#include "../include/papi_util.h"

void handler(int sig){
    fprintf(stderr, "Attack Task (%d) > Freeing memory\n", getpid());
    fflush(stderr);
    exit(0);
}

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

void attack_element_print(attack_list *al, attack_element *ae){
    int i, j, k;
    attack_element ae_tmp;
    attack_element *ae_tmp2;
    printf("-----------------\n");
    for(i=0;i<SIZE;i++){
        for(j=0;j<SIZE;j++){
            k = (int)(rand()%INDEX_SIZE);
            ae_tmp2 = al->al_index[k];
            memcpy(ae_tmp.ae_mat, ae->ae_mat, sizeof(ae->ae_mat));
            memcpy(ae->ae_mat, ae_tmp2->ae_mat, sizeof(ae->ae_mat));
            memcpy(ae_tmp2->ae_mat, ae_tmp.ae_mat, sizeof(ae->ae_mat));
            ae->ae_mat[i][j] += 10;
            printf("%g ",ae->ae_mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");

}

void attack_list_iterate(attack_list *al, attack_list *al_2){
    attack_element *iter;
    attack_element *iter_2;

    while(1){
        attack_list *al_3 = (attack_list*) malloc (sizeof(attack_list));
        al_3->al_index = (attack_element**)malloc(INDEX_SIZE * sizeof(attack_element*));
        attack_list_init(al_3);
        for(iter = al->al_head; iter; iter = iter->ae_next){
            for(iter_2 = al_2->al_head; iter_2; iter_2 = iter_2->ae_next){
                attack_element *iter_3;
                iter_3 = (attack_element*) malloc (sizeof(attack_element));
                memcpy(iter_3, iter, sizeof(attack_element));
                attack_list_add_elt(al_3, iter_3);
                attack_element_print(al, iter);
                attack_element_print(al_2, iter_2);
            }
        }
        attack_list_destroy(al_3);
    }
}


void attack_list_rand_iterate(attack_list *al){
    int i, j =0, k, cpt = 0;;
    while(1){
        srand(getpid()+i);
        i = (int)(rand()%al->al_nb_elements);
        k = (int)(rand()%10) + 1;
        printf("I = %d, K = %d\n", i, k);

        attack_element *iter = al->al_index[i];
        while((cpt != k) && (iter != NULL)){
            attack_element_print(al, iter);
            iter = iter->ae_next;
            cpt++;
        }
        j++;
        cpt = 0;
    }
}


int main(int argc, char **argv){

    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    sigaction(SIGUSR1, &sa, NULL);


    attack_list *al = (attack_list*)malloc(sizeof(attack_list));
    attack_list *al_2 = (attack_list*)malloc(sizeof(attack_list));
    al->al_index = (attack_element**)malloc(INDEX_SIZE *sizeof(attack_element*));
    al_2->al_index = (attack_element**)malloc(INDEX_SIZE * sizeof(attack_element*));
    
    //Add the process to the table used by the hypervisor    
    attack_list_init(al);
    attack_list_init(al_2);
    fprintf(stderr, "Attack task (%d) > al->nb_elts %d\n", getpid(), al->al_nb_elements);
    attack_list_add_n_elt(al);
    attack_list_add_n_elt(al_2);
    fprintf(stderr, "Attack task (%d) > Allocation done: al->nb_elts %d\n", getpid(), al->al_nb_elements);


    //Notify the scheduler
    kill(getppid(), SIGCONT);

    if(argc == 2){
        if(atoi(argv[1]) == 1){
            fprintf(stderr, "Attacker will use random iteration\n");
            attack_list_rand_iterate(al);
        }else{
            fprintf(stderr, "Attacker will use linear iteration\n");
            attack_list_iterate(al, al_2);
        }
    }else{
        fprintf(stderr, "Attacker will use linear iteration\n");
        attack_list_iterate(al, al_2);
    }

    attack_list_destroy(al);
    attack_list_destroy(al_2);

    return EXIT_SUCCESS;
}