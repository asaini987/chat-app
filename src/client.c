#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.h"

// Function to open a client file descriptor given an IP address and port
int open_clientfd(char *ip_address, int port) {
    int clientfd;
    struct sockaddr_in server_addr;

    // Create a socket
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error opening socket");
        return -1;
    }

    // Set server address details
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Convert IP address to binary form and set it in server address
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        perror("Error handling IP Address");
        return -1;
    }

    // Connect to the server
    if (connect(clientfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to socket");
        return -1;
    }

    return clientfd;
}