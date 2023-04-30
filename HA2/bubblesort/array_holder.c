#include "constants.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>


bool isSorted(int array[]){
    pid_t* child_pid;
    posix_spawn(child_pid, "./bubble", NULL, NULL, NULL, NULL);
    wait(*child_pid);

    for(int i = 0; i < N_ELEMS; i++){
        if(array[i] > array[i+1]){
            return false;
        }
    }
    return true;
}

void printArray(int array[]){
    for (int i = 0; i < N_ELEMS; i++){
        printf("%d ", array[i]);
    }

    printf("\n");
    sleep(0.001);
}


int main(){
    int shm_fd = shm_open(MEM_NAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(shm_fd, N_ELEMS);
    
    int* shared_array = mmap(NULL, N_ELEMS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *shared_array = *NUMBERS;

    while(!isSorted(shared_array)){
        printArray(shared_array);
    }
    
    printf("Process finished: \n");
    printArray(shared_array);

    close(shm_fd); shm_unlink(MEM_NAME); return 0;
}