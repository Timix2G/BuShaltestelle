#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT "80"
#define SEND_BUFFER_SIZE 512
#define RECV_BUFFER_SIZE 8192

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Verwendung: %s <Domain> <Pfad>\n", argv[0]);
        exit(0);
    }
    char *domain = argv[1];
    char *path = argv[2];



    // get address info (resolve hostname)
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(domain, PORT, &hints, &ai);
    if (status < 0) {
        fprintf(stderr, "error getting address info: %s\n", gai_strerror(status)); // no errno here :(
        exit(EXIT_FAILURE);
    }


    // create socket
    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) {
        perror("error creating socket"); // prints error set in errno
        freeaddrinfo(ai);
        exit(EXIT_FAILURE);
    }


    // set socket buffer sizes
    int sendBufferSize = SEND_BUFFER_SIZE;
    int recvBufferSize = RECV_BUFFER_SIZE;
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufferSize, sizeof(sendBufferSize)) < 0) {
        perror("error setting send buffer size"); // prints error set in errno
        freeaddrinfo(ai);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufferSize, sizeof(recvBufferSize)) < 0) {
        perror("error setting receive buffer size"); // prints error set in errno
        freeaddrinfo(ai);
        close(sockfd);
        exit(EXIT_FAILURE);
    }


    // establish TCP connection
    if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) != 0) {
        perror("error establishing connection"); // prints error set in errno
        freeaddrinfo(ai);
        close(sockfd);
        exit(EXIT_FAILURE);
    }


    // write request
    char *template = "GET %s HTTP/1.1\r\n"
                     "Host: %s\r\n\r\n";

    size_t request_len = snprintf(NULL, 0, template, path, domain) + 1; // calculate request length
    char *request = malloc(request_len); // malloc size of request
    if (request == NULL) {
        perror("error allocating space for request"); // prints error set in errno
        freeaddrinfo(ai);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    snprintf(request, request_len, template, path, domain); // actually write the request

    if (write(sockfd, request, strlen(request)) < 0) {
        perror("error writing request"); // prints error set in errno
        freeaddrinfo(ai);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    free(request);


    // read response
    char response[RECV_BUFFER_SIZE]; // create response buffer
    ssize_t bytes_received = read(sockfd, response, sizeof(response) -1); // read response

    if (bytes_received < 0) {
        perror("error reading response"); // prints error set in errno
        freeaddrinfo(ai);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    response[bytes_received] = 0; // set null terminator as last character


    // print response
    printf("%s", response);


    // free address info
    freeaddrinfo(ai);


    // close socket
    close(sockfd);


    return 0;
} 
