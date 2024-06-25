#include <stdlib.h>

#include "user.h"

struct user_list* user_list_init() {
    struct user_list* user_lst = (struct user_list*) malloc(sizeof(struct user_list));
    user_lst->head = NULL;
    user_lst->len = 0;
    pthread_mutex_init(&(user_lst->mutex), NULL);

    return user_lst;
}

void user_list_destroy(struct user_list* user_lst) {
    struct user_node* curr = user_lst->head;
    struct user_node* next_node;

    while (curr != NULL) {
        next_node = curr->next;
        free(curr->usr->username);
        free(curr->usr);
        free(curr);
        curr = next_node;
    }

    pthread_mutex_destroy(&(user_lst->mutex));
    free(user_lst);
}