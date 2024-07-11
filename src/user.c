#include <stdlib.h>
#include <string.h>

#include "user.h"

#define MAX_LOAD_FACTOR 0.75f // When the map's load factor reaches 0.75, it resizes

#define MAP_ADDITION 0
#define MAP_REMOVAL 1

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
        struct user_bucket* bucket = usr_map->table[i];

        if (pthread_rwlock_init(&(bucket->rw_lock), NULL) != 0) {
            perror("pthread_rwlock_init failed");
            exit(1);
        }

        bucket->head = NULL;
    }

    return usr_map;
}

void user_map_destroy(struct user_map* usr_map) {
    if (pthread_mutex_destroy(&(usr_map->mutex)) != 0) {
        perror("pthread_mutex_destroy failed");
        exit(1);
    }

    for (int i = 0; i < usr_map->capacity; i++) { // freeing every bucket
        struct user_bucket* bucket = usr_map->table[i];

        if (pthread_rwlock_destroy(&(bucket->rw_lock)) != 0) {
            perror("pthread_mutex_destroy failed in user_map_destroy");
            exit(1);
        }

        if (bucket->head == NULL) { // empty bucket
            continue;
        }

        struct user_node* curr = bucket->head;
        
        while (curr != NULL) { // freeing the linked list of nodes in each bucket
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

// FNV-1a Hash Algorithm
static uint32_t hash_username(const char* username, const int len) {
    uint32_t hash = 2166136261u;

    for (int i = 0; i < len; i++) {
        hash ^= (uint8_t) username[i];
        hash *= 16777619;
    }

    return hash;
}

static uint32_t get_index(struct user_map* map, char* username) {
    uint32_t hash = hash_username(username, strlen(username));

    if (pthread_mutex_lock(&(map->mutex)) != 0) {
        perror("pthread_mutex_lock failed");
        exit(1);
    }

    uint32_t idx = hash % map->capacity;

    if (pthread_mutex_unlock(&(map->mutex)) != 0) {
        perror("pthread_mutex_unlock failed");
        exit(1);
    }

    return idx;
}

static void rehash() {
    // TODO
}

static void update_map(struct user_map* map, int write_type) {
    if (pthread_mutex_lock(&(map->mutex)) != 0) {
        perror("pthread_mutex_lock failed");
        exit(1);
    }

    (write_type == MAP_ADDITION) ? map->size++ : map->size--;
    map->load_factor = (float) map->size / map->capacity;

    if (map->load_factor >= MAX_LOAD_FACTOR) {
        rehash();
    }

    if (pthread_mutex_unlock(&(map->mutex)) != 0) {
        perror("pthread_mutex_unlock failed");
        exit(1);
    }
}

int add_user(struct user_map* map, char* username, struct user* usr) {
    struct user_node* new_node = (struct user_node*) malloc(sizeof(struct user_node));
    new_node->usr = usr;
    new_node->next = NULL;

    uint32_t idx = get_index(map, username);
    struct user_bucket* bucket = map->table[idx];

    if (pthread_rwlock_wrlock(&(bucket->rw_lock)) != 0) {
        perror("pthread_rwlock_wrlock failed");
        exit(1);
    }

    if (bucket->head == NULL) { // empty bucket
        bucket->head = new_node;

        if (pthread_rwlock_unlock(&(bucket->rw_lock)) != 0) {
            perror("pthread_rwlock_unlock failed");
            exit(1);
        }
        
        update_map(map, MAP_ADDITION);
        return 0;
    }

    struct user_node* curr = bucket->head;

    while (curr != NULL) {
        if (strcmp(curr->usr->username, username) == 0) { // found a duplicate
            if (pthread_rwlock_unlock(&(bucket->rw_lock)) != 0) {
                perror("pthread_rwlock_unlock failed");
                exit(1);
            }

            free(new_node);
            return -1;
        }

        if (curr->next == NULL) { // found tail
            break;
        }

        curr = curr->next;
    }

    // username is not a duplicate
    curr->next = new_node;

    update_map(map, MAP_ADDITION);
    
    if (pthread_rwlock_unlock(&(bucket->rw_lock)) != 0) {
        perror("pthread_rwlock_unlock failed");
        exit(1);
    }

    return 0;
}

struct user* remove_user(struct user_map* map, char* username) {

}