#define _XOPEN_SOURCE 700 // ftruncate shenanigans

#include "constants.h"

#include <stdio.h>

// shm_open()
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

//memcpy()
#include <string.h>

// ftruncate()
#include <unistd.h>

//posix_spawn()
#include <spawn.h>

//waitpid()
#include <sys/wait.h>

const size_t MEM_SIZE = (size_t) N_ELEMS * sizeof(int);

#define SLEEP_TIME 0.25 // Interval in seconds


int arraySort(const int *array) {
  for (int i = 1; i < N_ELEMS; i++) {
    if(array[i] < array[i-1])
      return 0;
  }
  return 1;
}



int main() {

  // Open shared memory
  int shm_fd = shm_open(MEM_NAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if(shm_fd == -1) {
    perror("shm_open failed");
    return 1;
  }
  
  // Truncate shared memory
  if (ftruncate(shm_fd, (int64_t) MEM_SIZE) == -1) {
		perror("ftruncate failed");
		return 1;
	}
  
  
  // Map shared memory
  int *numbers = (int *)mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (numbers == MAP_FAILED) {
		perror("mmap MAP_FAILED (-1)");
		return 1;
	}
  
  //copy NUMBERS into shared memory
  memcpy(numbers, NUMBERS, MEM_SIZE);
  
  //define args for posix
  char const* args[] = { "./bubble", MEM_NAME, NULL};
  pid_t pid = getpid();

  int count = 0;

  while(!arraySort(numbers)) {
    
    //spawn child bubble
    int res = posix_spawn(&pid, *args, NULL, NULL,(char*const*)args, NULL);
    if(res != 0) {
      perror("posix failed");
    }

    //wait for it(in this case any child process) to end
    waitpid(0, NULL, 0);
    count++;
    // print array after one iteration of bubble
    printf("Iteration count:%d \n", count);
    for(int i = 0; i < N_ELEMS; i++)
      printf(" %d", numbers[i]);
    printf("\n");
    sleep(SLEEP_TIME);
  }

  // unmap 
  if (munmap(numbers, MEM_SIZE) == -1) {
		perror("munmap failed");
		return 1;
	}

	// Close shared memory
	if (close(shm_fd) == -1) {
		perror("close failed");
		return 1;
	}

	// Unlink shared memory
	if (shm_unlink(MEM_NAME) == -1) {
		perror("shm_unlink failed");
		return 1;
	}
  return 0; 
}

