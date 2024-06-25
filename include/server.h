#ifndef __SERVER
#define __SERVER

#include <pthread.h>
#include <signal.h>

/**
 * @brief Represents a connection to a client.
*/
struct connection {
    int connfd; // file descriptor used to read/write to a client
    char* haddrp; // domain name of the host (IP Address)
};

struct message {
    char* from;
    char* to;
    char buffer[300];
};

// flag to signal server clean up 
extern volatile sig_atomic_t shutdown_server; 

/**
 * @brief Sets flag to signal a graceful server shutdown.
 * 
 * @param sig The signal that was sent to this process.
*/
void shutdown_handler(int sig);

/**
 * @brief Opens a socket file descriptor to allow to listen for incoming connections.
 * 
 * @param port The port on which the server should execute on.
 * 
 * @return The listen file descriptor that can be used to accept client connections.
*/
int open_listenfd(int port);

/**
 * @brief Echoes the message back to the client. Just a test function.
 * 
 * Needs to be changed to relay messages to other clients.
 * 
 * @param connfd The file descriptor used to communicate with the client.
*/
void echo(int connfd);

/**
 * @brief Thread function to be executed by the thread pool to service a client.
 * 
 * @details Reads mesage and then relays it to the correct user(s).
 * 
 * @param conn The argument to the thread function.
 * 
 * @return Anything, including NULL.
*/
void* handle_client_connection(void* conn);

/**
 * @brief Sets a file descriptor to non-blocking.
 * 
 * @details Allows for server to not block on sockets.
 * 
 * @param fd The file descriptor to unblock.
*/
void set_nonblocking(int fd);

#endif