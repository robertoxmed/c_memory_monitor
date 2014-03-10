#ifndef __ATTACK_TASK_
#define __ATTACK_TASK_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define SIZE 3
#define INDEX_SIZE 400000

struct attack_element_t{
	double ae_mat[SIZE][SIZE];
	struct attack_element_t *ae_next;
	struct attack_element_t *ae_previous;
};

struct attack_list_t{
	int al_nb_elements;
	struct attack_element_t **al_index; //Index table with pointers to all elements
	struct attack_element_t *al_head;
	struct attack_element_t *al_tail;
};

typedef struct attack_list_t attack_list;

typedef struct attack_element_t attack_element;

void attack_list_init(attack_list *al);

void attack_list_destroy(attack_list *al);

void attac_element_init(attack_element *ae);

void attack_list_add_elt(attack_list *al, attack_element *ae);

void attack_list_add_n_elt(attack_list *al);

void attack_list_iterate(attack_list *al);

void attack_list_rand_iterate(attack_list *al);


#endif
