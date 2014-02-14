#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv){

    const PAPI_exe_info_t * prginfo = NULL;
    
    if(PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
        exit(1);

    if((prginfo = PAPI_get_executable_info()) == NULL)
        exit(1);

    printf("Start of user program is at %p\n", prginfo->address_info.text_start);
    printf("End of user program is at %p\n", prginfo->address_info.text_end);

    return 0;
}
