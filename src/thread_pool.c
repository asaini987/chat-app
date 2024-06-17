#include <stdlib.h>

#include "thread_pool.h" 

struct ThreadPool* thread_pool_init(int num_threads) { // add error checking
    struct ThreadPool* thread_pool = (struct ThreadPool*) malloc(sizeof(struct ThreadPool));

    thread_pool->threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
    thread_pool->thread_count = num_threads;

    struct TaskQueue* tk_queue = (struct TaskQueue*) malloc(sizeof(struct TaskQueue));
    tk_queue->head = NULL;
    tk_queue->tail = NULL;
    pthread_mutex_init(&(tk_queue->mutex), NULL);
    pthread_cond_init(&(tk_queue->cond), NULL);

    thread_pool->task_queue = tk_queue;

    return thread_pool;
}

void thread_pool_destroy(struct ThreadPool* thread_pool) {
    for (int i = 0; i < thread_pool->thread_count; i++) {
        pthread_join(thread_pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&(thread_pool->task_queue->mutex));
    pthread_cond_destroy(&(thread_pool->task_queue->cond));

    struct TaskNode* curr = thread_pool->task_queue->head;
    struct TaskNode* next_node;

    while (curr != NULL) {
        next_node = curr->next;
        free(curr->task);
        free(curr);
        curr = next_node;
    }
    
    free(thread_pool->task_queue);
    free(thread_pool);
}