#include <stdbool.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "server.h"
#include "task.h"
#include "thread_pool.h"
#include "user.h"

#define MAX_THREADS 10

int main(int argc, char* argv[]) {
    printf("Hello this is the main server\n");

    int listenfd; // Listening file descriptor
    int connfd; // Connection file descriptor
    int port; // Port number

    // Records address information of the client that connects to this server
    struct sockaddr_in client_addr;

    // Records length of client address
    socklen_t client_len;

    // Records the client host information through DNS
    // struct hostent* hp;

    // haddrp is used to remember the domain name of the host
    char* haddrp;

    // client port is the ephemeral port used by the client
    unsigned short client_port;

    // First check the command line arguments 
    if (argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        exit(0);
    }

    // Next, convert the port number string to an integer
    port = atoi(argv[1]);

    // Now create socket fd to listen for incoming connections
    listenfd = open_listenfd(port);
    // printf("%d\n", listenfd);

    printf("Echo Server is listening on port %d.\n", port);
    
    struct ThreadPool* thread_pool = thread_pool_init(MAX_THREADS);
    struct UserList* users = users_init();

    while (true) {
        client_len = sizeof(client_addr);
        printf("Echo server is accepting incoming connections on port %d.\n", port);
        connfd = accept(listenfd, (struct sockaddr*) &client_addr, &client_len);

        // convert network byte order IP addresses to dotted IP string
        haddrp = inet_ntoa(client_addr.sin_addr);

        // convert the port number from network byte order to host byte order
        client_port = ntohs(client_addr.sin_port);

        // print an information message
        printf(
            "Echo Server received a connection to (%s).\n"
            "Echo Server is using port %u and client has an ephemeral port of %u.\n",
            haddrp, port, client_port
        );

        struct Connection conn;
        conn.connfd = connfd;
        conn.haddrp = haddrp;

        // service the connection with a thread
        pthread_t tid;
        if ((pthread_create(&tid, NULL, handle_client_connection, (void*) &conn)) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    thread_pool_destroy(thread_pool);
    users_destroy(users);

    exit(0);
}