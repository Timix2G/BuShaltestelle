#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT "8080"
#define BUFFER_SIZE 1024
#define BACKLOG 10

static const char RESPONSE[] = "HTTP/1.0 200 OK\r\n\r\n<!doctype html><html><head><title>BuS 2023</title></head><body><h1>Gut gemacht!</h1><p>&#68;&#105;&#101;&#32;&#76;&#111;&#101;&#115;&#117;&#110;&#103;&#101;&#110;&#32;&#100;&#101;&#114;&#32;&#69;&#114;&#115;&#116;&#107;&#108;&#97;&#117;&#115;&#117;&#114;&#32;&#102;&#117;&#101;&#114;&#32;&#100;&#105;&#101;&#115;&#101;&#115;&#32;&#83;&#101;&#109;&#101;&#115;&#116;&#101;&#114;&#32;&#102;&#105;&#110;&#100;&#101;&#110;&#32;&#83;&#105;&#101;&#32;<a href=\"&#104;&#116;&#116;&#112;&#115;&#58;&#47;&#47;&#119;&#119;&#119;&#46;&#121;&#111;&#117;&#116;&#117;&#98;&#101;&#46;&#99;&#111;&#109;&#47;&#119;&#97;&#116;&#99;&#104;&#63;&#118;&#61;&#100;&#81;&#119;&#52;&#119;&#57;&#87;&#103;&#88;&#99;&#81;\">&#104;&#105;&#101;&#114;</a>&#46;&#32;&#66;&#105;&#116;&#116;&#101;&#32;&#116;&#101;&#105;&#108;&#101;&#110;&#32;&#83;&#105;&#101;&#32;&#100;&#105;&#101;&#32;&#76;&#111;&#101;&#115;&#117;&#110;&#103;&#101;&#110;&#32;&#110;&#105;&#99;&#104;&#116;&#32;&#109;&#105;&#116;&#32;&#97;&#110;&#100;&#101;&#114;&#101;&#110;&#46;</p></body></html>\r\n";

int shouldEnd = 0;

// SIGINT signal handler
void signal_handler(int signalNum) {
  if(signalNum == SIGINT) {
    printf("Server schliesst!\n");
    shouldEnd = 1;
  }
}

int main() {
  signal(SIGINT, signal_handler);

  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int gai, sockfd, new_fd;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  gai = getaddrinfo(NULL, PORT, &hints, &res);
  if (gai < 0) {
        fprintf(stderr, "error getting address info: %s\n", gai_strerror(gai)); // no errno here :(
        exit(EXIT_FAILURE);
    }

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd < 0) {
        perror("error creating socket"); // prints error set in errno
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
        perror("error establishing connection"); // prints error set in errno
        freeaddrinfo(res);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

  if(listen(sockfd, BACKLOG)) {
        perror("error listening"); // prints error set in errno
        close(sockfd);
        exit(EXIT_FAILURE);
    }
  
  while (!shouldEnd) {
    addr_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_fd < 0) {
            perror("error accepting connection");
            continue;
    }

    char req[BUFFER_SIZE];
    if (read(new_fd, req, sizeof(req)) < 0) {
            perror("error reading request"); // prints error set in errno
            close(new_fd);
            continue;
    }

    printf("Received: \n%s", req);
    
    if (write(new_fd, RESPONSE, sizeof(RESPONSE)) < 0) {
            perror("error writing response"); // prints error set in errno
            close(new_fd);
            continue;
    }

    printf("Wrote Response: \n%s", RESPONSE);

    close(new_fd);
  }
  
  freeaddrinfo(res);
  close(sockfd);

  return 0;
}
