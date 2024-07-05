#ifndef __USER
#define __USER

#include "message.h"

#define HASH_TABLE_CAPACITY 100

struct user {
    int connfd;
    char* username;
    struct message_queue* msg_queue;
};

struct user_node {
    struct user* usr;
    struct user_node* next;
};

struct user_list {
    struct user_node* head;
    pthread_mutex_t mutex;
};

/**
 * Thread-safe hash map to store their corresponding user structs.
 * Utilizes chaining to handle collisions.
 * Key-Value Pair: <char* username, struct user* usr>.
*/
struct user_map {
    struct user_list* users[HASH_TABLE_CAPACITY];
    int size;
    pthread_rwlock_t rw_lock;
};

struct user_map* user_map_init();
void add_user(struct user_map* map, struct user* usr);
void remove_user(struct user_map* map, struct user* usr);

#endif