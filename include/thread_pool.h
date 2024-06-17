#ifndef __THREAD_POOL
#define __THREAD_POOL

#include "task.h"

struct ThreadPool {
    pthread_t* threads;
    int thread_count;
    struct TaskQueue* task_queue;
};

struct ThreadPool* thread_pool_init(int num_threads);
void thread_pool_destroy(struct ThreadPool* thread_pool);

#endif