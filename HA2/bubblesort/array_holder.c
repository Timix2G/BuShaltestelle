#include "constants.h";
#include <sys/mman.h>;
#include <sys/stat.h>;
#include <fcntl.h>;
#include <unistd.h>;

int main(){
    int shm_fd = shm_open(*MEM_NAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(shm_fd, N_ELEMS);
    int* shared_array[] = mmap(NULL, N_ELEMS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *shared_array = NUMBERS;
    


    return 0;
}