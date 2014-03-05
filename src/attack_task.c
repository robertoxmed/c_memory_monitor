#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int fd;

int attaquant_write(int tempo){

    int ite=0;
    while(ite != tempo){
        
        printf("Writing in tests/attaquant.txt : %d\n", ite);

        if(write(fd, &ite, sizeof(int)) == -1){
            perror("write");
            exit(1);
        }

        //lseek(fd, (off_t)((float)rand()/ 100), SEEK_SET);

        ite++;
    }
    return 0;
}

int attaquant_seek(int tempo){

    int tmp;
    int ite = 0, status;
    struct stat buf;

    status = fstat(fd, &buf);
    if(status == -1){
        perror("stat");
        return 1;
    }

    while(ite != tempo){
        
        srand(getpid()+ite);

        lseek(fd, (off_t)((float)rand()/ buf.st_size), SEEK_SET);

        if(read(fd, &tmp, sizeof(int))==-1){
            perror("read");
            return 1;
        }
        printf("Read from tests/attaquant.txt %d\n", tmp);
        ite++;
    }
    return 0;
}

int main(int argc, char* argv[]){

    int res;

    if (argc != 3){
        printf("Usage: %s <nb iteration> <option>\n", argv[0]);
        return(EXIT_FAILURE);
    }

    if((fd = open("./tests/attaquant.txt", O_RDWR, 0600))== -1){
        perror("open");
        return(EXIT_FAILURE);
    }

    if (atoi(argv[2]) == 0)
        res = attaquant_write(atoi(argv[1]));
    else
        res = attaquant_seek(atoi(argv[1]));

    if (res){
        printf("error during the attack\n");
        return(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}
