#ifndef __RT_TASK_
#define __RT_TASK_

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define SIZE 3

struct rt_element_t{
	int mat[SIZE][SIZE];
	struct rt_element_t *next;
};

struct rt_list_t{
	timer_t tid; //Le timer de la tâche temps réel
	struct rt_element_t *head;
	struct rt_element_t *op1; //Les deux dernières cellules
	struct rt_element_t *op2;
};

typedef struct rt_element_t rt_element;
typedef struct rt_list_t rt_list;

//Initialise la liste chaînée avec deux éléments
void rt_task_init(rt_list *);

//Calcule le prochain élément de la liste
void rt_task_nextiter(rt_list *);

//Detruit la liste chaînée en libérant la mémoire
void rt_task_destroy(rt_list *);

#endif
