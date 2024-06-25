#include <stdlib.h>

#include "thread_pool.h" 

struct thread_pool* thread_pool_init(const int num_threads) { // add error checking
    struct thread_pool* tpool = (struct thread_pool*) malloc(sizeof(struct thread_pool));

    tpool->threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
    tpool->thread_cnt = num_threads;

    struct task_queue* tskq = (struct task_queue*) malloc(sizeof(struct task_queue));
    tskq->head = NULL;
    tskq->tail = NULL;
    pthread_mutex_init(&(tskq->mutex), NULL);
    pthread_cond_init(&(tskq->cond), NULL);

    tpool->tsk_queue = tskq;

    return tpool;
}

void thread_pool_destroy(struct thread_pool* tpool) {
    for (int i = 0; i < tpool->thread_cnt; i++) {
        pthread_join(tpool->threads[i], NULL);
    }

    pthread_mutex_destroy(&(tpool->tsk_queue->mutex));
    pthread_cond_destroy(&(tpool->tsk_queue->cond));

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