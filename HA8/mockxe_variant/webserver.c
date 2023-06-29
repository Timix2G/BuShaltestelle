#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define BACKLOG 10

static const char RESPONSE[] = "HTTP/1.0 200 OK\r\n\r\n<!doctype html><html><head><title>BuS 2023</title></head><body><h1>Gut gemacht!</h1><p>&#68;&#105;&#101;&#32;&#76;&#111;&#101;&#115;&#117;&#110;&#103;&#101;&#110;&#32;&#100;&#101;&#114;&#32;&#69;&#114;&#115;&#116;&#107;&#108;&#97;&#117;&#115;&#117;&#114;&#32;&#102;&#117;&#101;&#114;&#32;&#100;&#105;&#101;&#115;&#101;&#115;&#32;&#83;&#101;&#109;&#101;&#115;&#116;&#101;&#114;&#32;&#102;&#105;&#110;&#100;&#101;&#110;&#32;&#83;&#105;&#101;&#32;<a href=\"&#104;&#116;&#116;&#112;&#115;&#58;&#47;&#47;&#119;&#119;&#119;&#46;&#121;&#111;&#117;&#116;&#117;&#98;&#101;&#46;&#99;&#111;&#109;&#47;&#119;&#97;&#116;&#99;&#104;&#63;&#118;&#61;&#100;&#81;&#119;&#52;&#119;&#57;&#87;&#103;&#88;&#99;&#81;\">&#104;&#105;&#101;&#114;</a>&#46;&#32;&#66;&#105;&#116;&#116;&#101;&#32;&#116;&#101;&#105;&#108;&#101;&#110;&#32;&#83;&#105;&#101;&#32;&#100;&#105;&#101;&#32;&#76;&#111;&#101;&#115;&#117;&#110;&#103;&#101;&#110;&#32;&#110;&#105;&#99;&#104;&#116;&#32;&#109;&#105;&#116;&#32;&#97;&#110;&#100;&#101;&#114;&#101;&#110;&#46;</p></body></html>\r\n";

int shouldEnd = 0;

// SIGINT signal handler
void signal_handler(int signalNum) {
    if (signalNum == SIGINT) {
        printf("Server schliesst!\n");
        shouldEnd = 1;
    }
}

int main() {
    signal(SIGINT, signal_handler);

    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("error creating socket"); // prints error set in errno
        exit(EXIT_FAILURE);
    }

    // bind to address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("error binding to address"); // prints error set in errno
        close(sockfd);
        exit(EXIT_FAILURE);
    }


    // listen
    if(listen(sockfd, BACKLOG)) {
        perror("error listening"); // prints error set in errno
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    char addr_str[INET_ADDRSTRLEN];
    char port_str[5];
    inet_ntop(AF_INET, &(server_addr.sin_addr), addr_str,INET_ADDRSTRLEN);
    snprintf(port_str, 5, "%d", htons(server_addr.sin_port));

    printf("server up and listening on %s:%s ...\n", addr_str, port_str);

    // accept loop
    while (!shouldEnd) {

        // accept connection
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sockfd < 0) {
            perror("error accepting connection");
            continue;
        }

        printf("accepted new connection on socket %d\n", client_sockfd);


        // read request
        char request[BUFFER_SIZE]; // create request buffer
        ssize_t bytes_received = read(client_sockfd, request, sizeof(request) - 1); // read request

        if (bytes_received < 0) {
            perror("error reading request"); // prints error set in errno
            close(client_sockfd);
            continue;
        }

        request[bytes_received] = 0; // set null terminator as last character


        // print request
        printf("received: '%s'\n", request);


        // write response
        if (write(client_sockfd, RESPONSE, strlen(RESPONSE)) < 0) {
            perror("error writing response"); // prints error set in errno
            close(client_sockfd);
            continue;
        }


        // close socket afterward
        close(client_sockfd);

    }

    close(sockfd);

    return 0;
}
