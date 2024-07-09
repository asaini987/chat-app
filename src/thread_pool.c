#include <stdlib.h>
#include <stdio.h>

#include "thread_pool.h" 

void* worker_thread(void* t_arg) {
    struct task_queue* tsk_queue = (struct task_queue*) t_arg;

    while (1) {
        // thread will block here if queue is empty and still open
        struct task* tsk = dequeue_task(tsk_queue);

        if (tsk == NULL) { // the queue is closed and empty
            break;
        }

        (*(tsk->func))(tsk->arg); // execute the task
        free(tsk->arg);
        tsk->arg = NULL;
        free(tsk);
        tsk = NULL;
    }

    return NULL;
}

struct thread_pool* thread_pool_init(const int num_threads) {
    struct thread_pool* tpool = (struct thread_pool*) malloc(sizeof(struct thread_pool));

    if (tpool == NULL) {
        perror("malloc failed");
        exit(1);
    }

    // Initialize task queue and synchronization constructs
    struct task_queue* tskq = task_queue_init();

    tpool->tsk_queue = tskq;
    tpool->thread_cnt = num_threads;

    // Create array for threads
    tpool->threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);

    if (tpool->threads == NULL) {
        perror("malloc failed");
        exit(1);
    }

    // Spawn the threads
    for (int i = 0; i < tpool->thread_cnt; i++) {
        if (pthread_create(&(tpool->threads[i]), NULL, worker_thread, (void*) tpool->tsk_queue) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }

    return tpool;
}

void thread_pool_destroy(struct thread_pool* tpool) {
    // Set termination flag to stop the threads
    if (pthread_mutex_lock(&(tpool->tsk_queue->mutex)) != 0) {
        perror("pthread_mutex_lock failed");
        exit(1);
    }

    tpool->tsk_queue->is_closed = 1;

    if (pthread_mutex_unlock(&(tpool->tsk_queue->mutex)) != 0) {
        perror("pthread_mutex_unlock failed");
        exit(1);
    }
    
    // Reap the threads
    for (int i = 0; i < tpool->thread_cnt; i++) {
        if (pthread_join(tpool->threads[i], NULL) != 0) {
            perror("pthread_join failed");
            exit(1);
        }
    }

    free(tpool->threads);
    tpool->threads = NULL;

    // Destroy synchronization constructs
    if (pthread_mutex_destroy(&(tpool->tsk_queue->mutex)) != 0) {
        perror("pthread_mutex_destroy failed");
    }

    if (pthread_cond_destroy(&(tpool->tsk_queue->cond)) != 0) {
        perror("pthread_cond_destroy failed");
    }

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