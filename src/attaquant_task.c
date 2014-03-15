#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TAILLE_TAMPON 100

int fd;

int attaquant_write(int tempo){

    int ite=0;
    FILE *fic = NULL;

    while(ite != tempo){

        if(!(fic = fopen("./tests/attaquant.txt", "w"))){
            printf("error fopen\n");
            return(1);
        }

        if(fprintf(fic, "%d", ite)<=0){
            printf("error fprintf\n");
            return(1);
        }

        fclose(fic);
        ite++;
    }

    return 0;
}


int attaquant_seek(int tempo){

    char tampon[TAILLE_TAMPON];
    FILE *fic = NULL;
    int nb_read = 0;
    int ite = 0;

    if(!(fic = fopen("./tests/attaquant.txt", "r"))){
        printf("error fopen\n");
        return(1);
    }

    while(ite != tempo){
    
        printf("valeur de ite : %d \n", ite);

        if(!(nb_read = fread(tampon, sizeof(char),TAILLE_TAMPON,fic))){
            printf("error fread\n");
            return(1);
        }

        if (nb_read != TAILLE_TAMPON){
        //Retour au début du fichier jusqu'à tempo itération
            if (fseek(fic,0, SEEK_SET)){
	           printf("error fseek\n");
	           return(1);
            }   
            ite++;
        }else {
        //Avancée dans le fichier
            if (fseek(fic,TAILLE_TAMPON+1, SEEK_CUR)){
	           printf("error fseek\n");
	           return(1);
            }      
        }
    }

    fclose(fic);

    return 0;
}


int main(int argc, char* argv[]){

    int res;

    if (argc != 3){
        printf("Usage: %s <nb iteration> <option>\n", argv[0]);
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
