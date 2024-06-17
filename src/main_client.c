#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.h"

#define MAXLINE 300

int main(int argc, char* argv[]) {
    // client socket fd
    int clientfd;

    // port number
    int port;

    // stores IP address
    char* ip_address;

    // buffer to receive data
    char buf[MAXLINE];

    // check program args
    if (argc != 3) {
        fprintf(stderr, "usage: %s <IP Address> <port>\n", argv[0]);
        exit(0);
    }

    // first arg is IP address, second is port
    ip_address = argv[1];
    port = atoi(argv[2]);

    // open client socket descriptor given IP address and port
    if ((clientfd = open_clientfd(ip_address, port)) == -1) {
        exit(1);
    }

    // Print "type: " and fflush to stdout:
    printf("type: ");
    fflush(stdout);

    // continue to read into stdin until EOF
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        // Remove the newline from fgets:
        buf[strcspn(buf, "\n")] = '\0';

        ssize_t ns = send(clientfd, buf, strlen(buf), 0);
        ssize_t nr = recv(clientfd, buf, MAXLINE, 0);

        // Add a newline back on:
        buf[strlen(buf) + 1] = '\0';
        buf[strlen(buf)] = '\n';

        // Display and read again:
        printf("echo (%ld bytes): ", nr);
        fputs(buf, stdout);
        printf("type: ");
        fflush(stdout);
    }

    // Close the file descriptor:
    close(clientfd);
    exit(0);
}


