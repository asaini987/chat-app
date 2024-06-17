#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "server.h"

#define MAXLINE 300 // max text length
#define LISTENQ 5 // second arg to listen()

int open_listenfd(int port) {
    int listenfd; // socket file descriptor for server to listen on
    int optval = 1;
    struct sockaddr_in server_addr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating server socket\n");
        return -1;
    }

    // Eliminates "Address already in use" error from bind
    if ((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*) &optval, sizeof(int))) < 0) {
        perror("Error with sockopt\n");
        return -1;
    }

    bzero((char*) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    // allow connections from ANY IP address
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short) port);

    if (bind(listenfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        return -1;
    }

    if ((listen(listenfd, LISTENQ)) < 0) {
        return -1;
    }

    return listenfd;
}

void echo(int connfd) {
    size_t n;
    char buf[MAXLINE];

    // keep reading until client closes connection
    while ((n = recv(connfd, buf, MAXLINE, 0)) != 0) {
        buf[n] = '\0';
        printf("Echo Server received %lu (%s) incoming bytes.\n", n, buf);
        printf("Echo Server is sending %lu bytes back to client (%s).\n", n, buf);
        send(connfd, buf, n, 0);
    }
}

void* handle_client_connection(void* conn) {
    pthread_detach(pthread_self());
    struct Connection* c = (struct Connection*) conn;

    echo(c->connfd);
    printf("Echo Server is closing the connection on (%s).\n", c->haddrp);

    // close the connection
    close(c->connfd);
    return NULL;
}