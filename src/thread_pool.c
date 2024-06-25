#include <stdlib.h>

#include "thread_pool.h" 

struct thread_pool* thread_pool_init(const int num_threads) { // add error checking
    struct thread_pool* tpool = NULL;
    tpool = (struct thread_pool*) malloc(sizeof(struct thread_pool));

    if (tpool == NULL) {
        perror("malloc() failed");
        exit(1);
    }

    // Create memory for threads
    tpool->threads = NULL;
    tpool->threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);

    if (tpool->threads == NULL) {
        perror("malloc() failed");
        free(tpool);
        exit(1);
    }

    tpool->thread_cnt = num_threads;

    // Initialize task queue and synchronization constructs
    struct task_queue* tskq = NULL;
    tskq = (struct task_queue*) malloc(sizeof(struct task_queue));

    if (tskq == NULL) {
        perror("malloc() failed");
        free(tpool->threads);
        free(tpool);
        exit(1);
    }

    tskq->head = NULL;
    tskq->tail = NULL;

    if (pthread_mutex_init(&(tskq->mutex), NULL) != 0) {
        perror("pthread_mutex_init() failed");
        free(tskq);
        free(tpool->threads);
        free(tpool);
        exit(1);
    }

    if (pthread_cond_init(&(tskq->cond), NULL) != 0) {
        perror("pthread_cond_init() failed");
        free(tskq);
        free(tpool->threads);
        free(tpool);
        exit(1);
    }

    tpool->tsk_queue = tskq;

    return tpool;
}

void thread_pool_destroy(struct thread_pool* tpool) {
    // Reap all threads
    for (int i = 0; i < tpool->thread_cnt; i++) {
        pthread_join(tpool->threads[i], NULL);
    }

    // Destroy synchronization constructs
    pthread_mutex_destroy(&(tpool->tsk_queue->mutex));
    pthread_cond_destroy(&(tpool->tsk_queue->cond));

    // Free entire task queue and its nodes
    struct task_node* curr = tpool->tsk_queue->head;
    struct task_node* next_node;

    while (curr != NULL) {
        next_node = curr->next;
        free(curr->tsk);
        free(curr);
        curr = next_node;
    }
    
    free(tpool->tsk_queue);
    free(tpool);
}