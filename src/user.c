#include <stdlib.h>

#include "user.h"

struct UserList* users_init() {
    struct UserList* users = (struct UserList*) malloc(sizeof(struct UserList));
    users->head = NULL;
    users->len = 0;
    pthread_mutex_init(&(users->mutex), NULL);

    return users;
}

void users_destroy(struct UserList* users) {
    struct UserNode* curr = users->head;
    struct UserNode* next_node;

    while (curr != NULL) {
        next_node = curr->next;
        free(curr->user->username);
        free(curr->user);
        free(curr);
        curr = next_node;
    }

    pthread_mutex_destroy(&(users->mutex));
    free(users);
}