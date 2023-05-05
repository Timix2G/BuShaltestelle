#include "constants.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const size_t MEM_SIZE = (size_t) N_ELEMS * sizeof(int);

void sort(int *array){
    //einfaches bubblesort ohne beachtung der schon sortierten Elemente
    for (int i = 0; i < N_ELEMS - 1; i++){

        if(array[i] > array[i + 1]){
            int temp = array[i];
            array[i] = array[i+1];
            array[i+1] = temp;
        }
    }
}

int main(){

    //auf shared-memory zugreifen und array aus memory laden
    int shm_fd = shm_open(MEM_NAME, O_RDWR, 0);
    int* shared_array = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sort(shared_array);  

    //Verbindungen schließen
    munmap(shared_array, MEM_SIZE); close(shm_fd);
    return 0;
}