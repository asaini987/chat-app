#include <stdlib.h>
#include <string.h>

#include "user.h"

#define MAX_LOAD_FACTOR 0.75

struct user_map* user_map_init() {
    struct user_map* usr_map = (struct user_map*) malloc(sizeof(struct user_map));

    if (usr_map == NULL) {
        perror("malloc failed");
        exit(1);
    }

    usr_map->capacity = 100;
    usr_map->size = 0;
    usr_map->load_factor = 0.0f;
    
    if (pthread_mutex_init(&(usr_map->mutex), NULL) != 0) {
        perror("pthread_mutex_init failed");
        exit(1);
    }

    usr_map->table = (struct user_bucket*) malloc(sizeof(struct user_bucket) * usr_map->capacity);

    for (int i = 0; i < usr_map->capacity; i++) {
        usr_map->table[i] = NULL;
    }

    return usr_map;
}

void message_queue_destroy(struct message_queue* msg_queue);

void user_map_destroy(struct user_map* usr_map) {
    if (pthread_mutex_destroy(&(usr_map->mutex)) != 0) {
        perror("pthread_mutex_destroy failed");
        exit(1);
    }

    struct user_bucket* bucket;
    for (int i = 0; i < usr_map->capacity; i++) {
        if (usr_map->table[i] == NULL) { // empty bucket
            continue;
        }

        bucket = usr_map->table[i];

        if (pthread_rwlock_destroy(&(bucket->rw_lock)) != 0) {
            perror("pthread_mutex_destroy failed in user_map_destroy");
            exit(1);
        }

        struct user_node* curr = bucket->head;
        
        while (curr != NULL) {
            struct user* usr = curr->usr;
            message_queue_destroy(&(usr->msg_queue));
            free(usr->username);
            close(usr->connfd);
            free(usr);
            
            struct user_node* next_node = curr->next;
            free(curr);
            curr = next_node;
        }


    }
}

static uint32_t hash_username(const char* username, const int len) {
    uint32_t hash = 2166136261u;

    for (int i = 0; i < len; i++) {
        hash ^= (uint8_t) username[i];
        hash *= 16777619;
    }

    return hash;
}