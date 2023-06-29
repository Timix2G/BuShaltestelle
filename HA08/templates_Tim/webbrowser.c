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

  gai = getaddrinfo(domain, "80", &hints, &result);
  if(gai != 0){
    printf("fehler bei getaddrinfo");
    exit(0);
  }

  sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if(sockfd == -1){
    printf("fehler bei Socket erstellung");
    exit(0);
  }

  connect(sockfd, result->ai_addr, result->ai_addrlen);
  printf("%s", httpstr);
  write(sockfd, httpstr, sizeof(httpstr));
  read(sockfd, resStr, sizeof(resStr));
  printf("%s", *resStr);
  freeaddrinfo(result);
  free(httpstr);
  return 0;
} 
