#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT "80"
#define SEND_BUFFER_SIZE 512
#define RECV_BUFFER_SIZE 8192

int main(int argc, char* argv[]) {
  if(argc != 3){
    printf("Verwendung: %s <Domain> <Pfad>\n", argv[0]);     
    exit(0);
  }
  char *domain = argv[1];
  char *path = argv[2];
  
  // TODO
  
  return 0;
} 
