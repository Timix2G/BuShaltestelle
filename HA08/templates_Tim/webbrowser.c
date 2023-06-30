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
  char *httpstr = (char*)malloc(SEND_BUFFER_SIZE * sizeof(char));
  char resStr[RECV_BUFFER_SIZE];
  struct addrinfo hints, *result;
  int gai, sockfd;

  sprintf(httpstr, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  gai = getaddrinfo(domain, PORT, &hints, &result);
  if (gai < 0) {
        fprintf(stderr, "error getting address info: %s\n", gai_strerror(gai)); // no errno here :(
        exit(EXIT_FAILURE);
    }

  sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (sockfd < 0) {
        perror("error creating socket"); // prints error set in errno
        freeaddrinfo(result);
        exit(EXIT_FAILURE);
  }

  if (connect(sockfd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("error establishing connection"); // prints error set in errno
        freeaddrinfo(result);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
  printf("Request: \n%s", httpstr);

  if (write(sockfd, httpstr, strlen(httpstr)) < 0) {
        perror("error writing request"); // prints error set in errno
        freeaddrinfo(result);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
  free(httpstr);

  if (read(sockfd, resStr, sizeof(resStr) -1) < 0) {
        perror("error reading response"); // prints error set in errno
        freeaddrinfo(result);
        close(sockfd);
        exit(EXIT_FAILURE);
  }
  printf("Response: \n%s", resStr);
  freeaddrinfo(result);
  close(sockfd);

  FILE *fptr = fopen("./build/test.html", "w");
  if (fputs(resStr, fptr) < 0) {
        perror("error writing to file"); // prints error set in errno
        fclose(fptr);
        exit(EXIT_FAILURE);
  }
  fclose(fptr);
  return 0;
} 
