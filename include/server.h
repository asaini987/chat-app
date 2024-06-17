#ifndef __SERVER
#define __SERVER

#include <pthread.h>

struct Connection {
    int connfd;
    char* haddrp; // domain name of the host (IP Address)
};

struct Message {
    char* from;
    char* to;
    char buffer[300];
};

int open_listenfd(int port);
void echo(int connfd);
void* handle_client_connection(void* conn);

#endif