#ifndef __USER
#define __USER

#include <pthread.h>

struct User {
    int connfd;
    char* username;
};

struct UserNode {
    struct User* user;
    struct UserNode* next;
};

struct UserList {
    struct UserNode* head;
    int len;
    pthread_mutex_t mutex;
};

struct UserList* users_init();
void users_destroy(struct UserList* users);

void add_user(struct User* user);
void remove_user(int connfd);

#endif