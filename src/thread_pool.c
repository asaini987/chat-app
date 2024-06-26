#include <stdlib.h>
#include <stdio.h>

#include "thread_pool.h" 

// Termination flag to signal the pool's threads to exit their function
static int stop_threads = 0;

struct thread_pool* thread_pool_init(const int num_threads) {
    struct thread_pool* tpool = (struct thread_pool*) malloc(sizeof(struct thread_pool));

    if (tpool == NULL) {
        perror("malloc() failed");
        exit(1);
    }

    // Initialize task queue and synchronization constructs
    struct task_queue* tskq = (struct task_queue*) malloc(sizeof(struct task_queue));

    if (tskq == NULL) {
        perror("malloc() failed");
        exit(1);
    }

    tskq->head = NULL;
    tskq->tail = NULL;

    if (pthread_mutex_init(&(tskq->mutex), NULL) != 0) {
        perror("pthread_mutex_init() failed");
        exit(1);
    }

    if (pthread_cond_init(&(tskq->cond), NULL) != 0) {
        perror("pthread_cond_init() failed");
        exit(1);
    }

    tpool->tsk_queue = tskq;
    tpool->thread_cnt = num_threads;

    // Create array for threads
    tpool->threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);

    if (tpool->threads == NULL) {
        perror("malloc() failed");
        exit(1);
    }

    // Start the threads
    for (int i = 0; i < tpool->thread_cnt; i++) {
        if (pthread_create(&tpool->threads[i], NULL, worker_thread, (void*) tpool->tsk_queue) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    return tpool;
}

void thread_pool_destroy(struct thread_pool* tpool) {
    // Set termination flag and reap all threads
    stop_threads = 1;
    for (int i = 0; i < tpool->thread_cnt; i++) {
        if (pthread_join(tpool->threads[i], NULL) != 0) {
            perror("pthread_join() failed");
            exit(1);
        }
    }

    free(tpool->threads);
    tpool->threads = NULL;

    // Destroy synchronization constructs
    pthread_mutex_destroy(&(tpool->tsk_queue->mutex));
    pthread_cond_destroy(&(tpool->tsk_queue->cond));

    // Free entire task queue and its nodes
    struct task_node* curr = tpool->tsk_queue->head;
    struct task_node* next_node;

    while (curr != NULL) {
        next_node = curr->next;
        free(curr->tsk);
        curr->tsk = NULL;
        free(curr);
        curr = next_node;
    }
    
    free(tpool->tsk_queue);
    tpool->tsk_queue = NULL;
    free(tpool);
}

void* worker_thread(void* t_arg) {
    struct task_queue* tsk_queue = (struct task_queue*) t_arg;

    while (stop_threads != 0) {
        struct task* tsk = dequeue_task(tsk_queue);
        tsk->func(tsk->arg); // execute the task
        free(tsk);
    }

    return NULL;
}