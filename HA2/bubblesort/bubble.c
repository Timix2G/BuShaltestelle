#include "constants.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
    int shm_fd = shm_open(MEM_NAME, O_RDWR, 0);
    int* shared_array = mmap(NULL, N_ELEMS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    for (int i = 0; i < N_ELEMS; i++){

        if(shared_array[i + 1] < shared_array[i]){
            int temp = shared_array[i];
            shared_array[i] = shared_array[i+1];
            shared_array[i+1] = temp;
        }
    }
    

    munmap(shared_array, N_ELEMS); close(shm_fd);
    return 0;
}