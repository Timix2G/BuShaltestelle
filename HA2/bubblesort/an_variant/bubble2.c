#include <stdio.h>

// shm_open()
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "constants.h"

// close()
#include <unistd.h>

const size_t MEM_SIZE = (size_t) N_ELEMS * sizeof(int);

int main() {
	// Open shared memory
	int shm_fd = shm_open(MEM_NAME, O_RDWR, 0);
	if (shm_fd == -1) {
		perror("shm_open failed");
		return 1;
	}

	// Map shared memory
	int *shm_ptr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) {
		perror("mmap failed");
		return 1;
	}
	
	// Make one iteration of bubble sort through the array
	for (int i = 1; i < N_ELEMS; i++) {
		if (shm_ptr[i] < shm_ptr[i-1]) {
			int temp = shm_ptr[i];
			shm_ptr[i] = shm_ptr[i-1];
			shm_ptr[i-1] = temp;
		}
	}

	// Unmap shared memory
	if (munmap(shm_ptr, MEM_SIZE) == -1) {
		perror("munmap failed");
		return 1;
	}

	// Close shared memory
	if (close(shm_fd) == -1) {
		perror("close failed");
		return 1;
	}

	return 0;
}