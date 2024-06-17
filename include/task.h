#ifndef __TASK
#define __TASK

#include <pthread.h>

struct Task {
    void (*func) (void*);
    void* arg;
};

struct TaskNode {
    struct Task* task;
    struct TaskNode* next;
};

struct TaskQueue {
    struct TaskNode* head;
    struct TaskNode* tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void enqueue_task(struct TaskQueue* queue, struct Task* task);
struct Task* dequeue_task(struct TaskQueue*);

#endif