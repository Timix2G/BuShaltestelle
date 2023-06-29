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
  char *resStr[RECV_BUFFER_SIZE];
  struct addrinfo hints, *result;
  int gai, sockfd;

  sprintf(httpstr, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  gai = getaddrinfo(domain, PORT, &hints, &result);
  if(gai != 0){
    printf("fehler bei getaddrinfo");
    exit(0);
  }

  sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if(sockfd == -1){
    printf("fehler bei Socket erstellung");
    exit(0);
  }

  /*set socket buffer sizes
    int sendBufferSize = SEND_BUFFER_SIZE;
    int recvBufferSize = RECV_BUFFER_SIZE;
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufferSize, sizeof(sendBufferSize)) < 0) {
        perror("error setting send buffer size"); // prints error set in errno
        freeaddrinfo(result);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufferSize, sizeof(recvBufferSize)) < 0) {
        perror("error setting receive buffer size"); // prints error set in errno
        freeaddrinfo(result);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
  */

  if (connect(sockfd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("error establishing connection"); // prints error set in errno
        freeaddrinfo(result);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

  printf("%s", httpstr);

  if (write(sockfd, httpstr, strlen(httpstr)) < 0) {
        perror("error writing request"); // prints error set in errno
        freeaddrinfo(result);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    free(httpstr);

  ssize_t bytes_received = read(sockfd, resStr, sizeof(resStr) -1); // read response

    if (bytes_received < 0) {
        perror("error reading response"); // prints error set in errno
        freeaddrinfo(result);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    resStr[bytes_received] = 0; // set null terminator as last character
  printf("%s", *resStr);
  freeaddrinfo(result);
  close(sockfd);
  return 0;
} 
