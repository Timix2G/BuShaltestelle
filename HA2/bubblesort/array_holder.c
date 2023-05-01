#include "constants.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>


bool isSorted(int array[]){

    //Kindprozess erzeugen (hoffentlich)
    int pidBuffer;
    int* ptr = &pidBuffer;
    char const* args[] = {"./bubble", MEM_NAME, NULL};
    posix_spawn(ptr, *args, NULL, NULL, (char*const*)args, __environ);
    
    if(pidBuffer <= 0){
        printf("Kindprozess konnte nicht erstellt oder pid nicht in den Buffer geschrieben werden\n");
    }else{
        wait(pidBuffer);
    }

    //auf Sortierung pruefen
    for(int i = 0; i < N_ELEMS; i++){
        if(array[i] > array[i+1]){
            return false;
        }
    }
    return true;
}

void printArray(int array[]){
    //einfache Array-Ausgabe
    for (int i = 0; i < N_ELEMS; i++){
        printf("%d ", array[i]);
    }

    printf("\n");
}

void fillArray(int array[]){
    //einfaches fuellen des Arrays
    for(int i = 0; i < N_ELEMS; i++){
        array[i] = NUMBERS[i];
    }
}


int main(){
    //shared-memory oeffnen und auf NUMBERS vergroessern
    int shm_fd = shm_open(MEM_NAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(shm_fd, sizeof(NUMBERS));
    
    //das Array auf das shared-memory laden
    int* shared_array = mmap(NULL, sizeof(NUMBERS), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    fillArray(shared_array);

    //auf sortierung pruefen
    while(!isSorted(shared_array)){
        printArray(shared_array);
    }
    
    //fertig
    printf("Process finished: \n");
    printArray(shared_array);

    //Verbindungen schließen
    close(shm_fd); shm_unlink(MEM_NAME); return 0;
}