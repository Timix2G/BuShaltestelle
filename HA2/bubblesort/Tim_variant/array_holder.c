#include "constants.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#define _XOPEN_SOURCE 700 


bool isSorted(const int *array){

    //auf Sortierung pruefen
    for(int i = 0; i < N_ELEMS - 1; i++){
        if(array[i] > array[i+1]){
            return false;
        }
    }
    return true;
}

void printArray(const int *array){
    
    //einfache Array-Ausgabe
    for (int i = 0; i < N_ELEMS; i++){
        printf("%d ", array[i]);
    }

    printf("\n");
}

int main(){
    //shared-memory oeffnen und auf NUMBERS vergroessern
    int shm_fd = shm_open(MEM_NAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(shm_fd, sizeof(NUMBERS));
    
    //das Array auf das shared-memory laden
    int* shared_array = mmap(NULL, sizeof(NUMBERS), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    memcpy(shared_array, NUMBERS, sizeof(NUMBERS));

    pid_t pidBuffer = getpid();
    char const* args[] = {"./bubble", MEM_NAME, NULL};
    
    int count = 1;
    //auf sortierung pruefen
    while(!isSorted(shared_array)){
        //Kindprozess erzeugen (hoffentlich)
        
        int res = posix_spawn(&pidBuffer, *args, NULL, NULL, (char*const*)args, NULL);
        if(res != 0){
            perror("posix failed");
        }

        waitpid(0, NULL, 0);
        printf("Iteration %d: ", count++);
        printArray(shared_array);
    }
    
    //fertig
    printf("Process finished: \n");
    printArray(shared_array);

    //Verbindungen schließen
    close(shm_fd); shm_unlink(MEM_NAME); return 0;
}