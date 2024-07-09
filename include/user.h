#ifndef __USER
#define __USER

#include "message.h"

#define MAX_LOAD_FACTOR 0.75f

struct user {
    int connfd;
    char* username;
    struct message_queue* msg_queue;
};

struct user_node {
    struct user* usr;
    struct user_node* next;
};

struct user_bucket {
    struct user_node* head;
    pthread_rwlock_t rw_lock;
};

/**
 * Thread-safe hash map to store their corresponding user structs.
 * Utilizes chaining to handle collisions.
 * Key-Value Pair: <char* username, struct user* usr>.
*/
struct user_map {
    struct user_bucket** table;
    int size;
    int capacity;
    float load_factor;
    pthread_mutex_t mutex;
};

struct user_map* user_map_init();
void user_map_destroy(struct user_map* usr_map);

int add_user(struct user_map* map, struct user* usr);
void remove_user(struct user_map* map, struct user* usr);

#endif