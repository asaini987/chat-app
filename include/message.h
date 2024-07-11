#ifndef __MESSAGE
#define __MESSAGE

#include <pthread.h>

#define MSG_SIZE 1024

struct message {
    char* author;
    char* recipient;
    char text[MSG_SIZE];
};

struct message_node {
    struct message* msg;
    struct message_node* next;
};

struct message_queue {
    struct message_node* head;
    struct message_node* tail;
    pthread_mutex_t mutex;
};

void message_queue_destroy(struct message_queue* msg_queue);

#endif