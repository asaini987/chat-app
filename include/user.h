#ifndef __USER
#define __USER

#include <pthread.h>

struct user {
    int connfd;
    char* username;
};

struct user_node {
    struct user* usr;
    struct user_node* next;
};

struct user_list {
    struct user_node* head;
    int len;
    pthread_mutex_t mutex;
};

struct user_list* user_list_init();
void user_list_destroy(struct user_list* user_lst);

void add_user(struct user* new_user);
void remove_user(int connfd);

#endif