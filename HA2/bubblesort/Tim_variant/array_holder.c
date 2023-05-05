#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 199309L

#include "constants.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

const size_t MEM_SIZE = (size_t) N_ELEMS * sizeof(int);

bool isSorted(const int *array){

    for(int i = 0; i < N_ELEMS - 1; i++){
        if(array[i] > array[i+1]){
            return false;
        }
    }
    
    return true;
}

void printArray(const int *array){
    
    for (int i = 0; i < N_ELEMS; i++){
        printf("%d ", array[i]);
    }

    printf("\n");
}

int main(){
    //shared-memory oeffnen
    int shm_fd = shm_open(MEM_NAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  
    //shm auf die groesse von NUMBERS zuschneiden
    ftruncate(shm_fd, MEM_SIZE);
    
    //das Array auf shm laden
    int* shared_array = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    memcpy(shared_array, NUMBERS, MEM_SIZE);
    
    int count = 1;

    while(!isSorted(shared_array)){

        //pruefe alle 500.000.000 ns (0.5 Sekunden)
        nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);

        printf("Check %d: ", count++); printArray(shared_array);
    }
    
    printf("Process finished.");

    //Shm verbindung und fd schliessen
    shm_unlink(MEM_NAME); close(shm_fd); return 0;
}