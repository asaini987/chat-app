#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include <errno.h>

#include "server.h"
#include "task.h"
#include "thread_pool.h"
#include "user.h"

#define MAX_THREADS 7
#define MAX_EVENTS 51

int main(int argc, char* argv[]) {
    printf("Hello this is the main server\n");

    // register signal handler for SIGINT
    struct sigaction sa;
    sigemptyset(&(sa.sa_mask));
    sa.sa_handler = shutdown_handler;
    sa.sa_flags = 0;

    if ((sigaction(SIGINT, &sa,  NULL)) == -1) {
        perror("Failed to register signal handler");
        exit(1);
    }

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
    
    struct thread_pool* tpool = thread_pool_init(MAX_THREADS);
    // struct user_list* users = users_init();

    struct kevent chlist[MAX_EVENTS]; // events to monitor
    struct kevent evlist[MAX_EVENTS]; // events that were triggered
    // TODO: populate arrays

    int kq = kqueue();

    if (kq == -1) {
        perror("Failed to make kqueue");
        exit(1);
    }
    
    // initialize kevent struct for monitoring listenfd for READ
    EV_SET(&chlist[0], listenfd, EVFILT_READ, EV_ADD, 0, 0, 0);
    int num_changes = 1; // number of changes to monitor
    int nev; // number of events triggered
    
    while (1) { // fix server shutdown
        if (shutdown_server == 1) {
            close(listenfd);
            puts("Echo server is closing connections and shutting down");
            break;
        }

        client_len = sizeof(client_addr);
        printf("Echo server is accepting incoming connections on port %d.\n", port);

        nev = kevent(kq, chlist, num_changes, evlist, MAX_EVENTS, NULL); // blocks indefinetly

        if (nev == -1) {
            perror("kevent()");
            // interrupted system call
            if (errno == EINTR) { // Ctrl ^C was pressed, shutdown the server
                continue;
            }
            
            exit(1);
        }

        if (nev > 0) {
            for (int i = 0; i < nev; i++) { // iterate thru evlist
                if (evlist[i].ident == (uintptr_t) listenfd) {
                    // accept() can be called
                    connfd = accept(listenfd, (struct sockaddr*) &client_addr, &client_len);

                    // set_nonblocking(connfd);

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

                    struct connection conn;
                    conn.connfd = connfd;
                    conn.haddrp = haddrp;

                    // service the connection with a thread
                    pthread_t tid;
                    if ((pthread_create(&tid, NULL, handle_client_connection, (void*) &conn)) != 0) {
                        perror("Failed to create thread");
                        exit(1);
                    }
                }
            }
        }

        // connfd = accept(listenfd, (struct sockaddr*) &client_addr, &client_len);
        // set_nonblocking(connfd);

        // // convert network byte order IP addresses to dotted IP string
        // haddrp = inet_ntoa(client_addr.sin_addr);

        // // convert the port number from network byte order to host byte order
        // client_port = ntohs(client_addr.sin_port);

        // // print an information message
        // printf(
        //     "Echo Server received a connection to (%s).\n"
        //     "Echo Server is using port %u and client has an ephemeral port of %u.\n",
        //     haddrp, port, client_port
        // );

        // struct connection conn;
        // conn.connfd = connfd;
        // conn.haddrp = haddrp;

        // // service the connection with a thread
        // pthread_t tid;
        // if ((pthread_create(&tid, NULL, handle_client_connection, (void*) &conn)) != 0) {
        //     perror("Failed to create thread");
        //     exit(1);
        // }
    }

    puts("destroying thread pool");
    thread_pool_destroy(tpool);
    // users_destroy(users);

    exit(0);
}