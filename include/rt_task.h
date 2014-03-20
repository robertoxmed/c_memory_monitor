#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZE 32768

//Initialise les deux tableaux
void rt_task_init(int[SIZE], int[SIZE]);

//Traitement sur les tableaux
void rt_task_treatement(int[SIZE], int[SIZE], int);
