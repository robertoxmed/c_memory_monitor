#ifndef __RT_TASK_
#define __RT_TASK_

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdblib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

typedef struct rt_task{
    timer_t rt_timer;
    int fd_w;
}


