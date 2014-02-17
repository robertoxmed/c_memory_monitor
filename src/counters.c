#include <papi.h>
#include <stdio.h>
#include <stdlib.h>


int main (int argc, char ** argv){
    
    int num_hwcntrs = 0;

    if((num_hwcntrs = PAPI_num_counters()) <= PAPI_OK)
        exit(1);

    printf("This system has %d available counters\n", num_hwcntrs);
    

    return 0;
}
