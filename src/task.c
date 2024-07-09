#include <stdlib.h>
#include <stdio.h>

#include "task.h"

struct task_queue* task_queue_init() {
    struct task_queue* tskq = (struct task_queue*) malloc(sizeof(struct task_queue));

    if (tskq == NULL) {
        perror("malloc failed");
        exit(1);
    }

    tskq->head = NULL;
    tskq->tail = NULL;
    tskq->is_closed = 0;

    if (pthread_mutex_init(&(tskq->mutex), NULL) != 0) {
        perror("pthread_mutex_init failed");
        exit(1);
    }

    if (pthread_cond_init(&(tskq->cond), NULL) != 0) {
        perror("pthread_cond_init failed");
        exit(1);
    }

    return tskq;
}

void task_queue_destroy(struct task_queue* tskq) {
    
}

void enqueue_task(struct task_queue* queue, struct task* new_task) {
    struct task_node* node = (struct task_node*) malloc(sizeof(struct task_node));

    if (node == NULL) {
        perror("malloc() failed");
        exit(1);
    }

    // Initialize the new task node
    node->tsk = new_task;
    node->next = NULL;

    if (pthread_mutex_lock(&(queue->mutex)) != 0) {
        perror("pthread_mutex_lock() failed");
        exit(1);
    }

    // Do not add tasks to queue when it is closed
    if (queue->is_closed) {
        if (pthread_mutex_unlock(&(queue->mutex)) != 0) {
            perror("pthread_mutex_unlock failed");
            exit(1);
        }
        
        return;
    }

    // if adding as first element, set as head, else add onto tail
    if (queue->head == NULL) {
        queue->head = node;
    } else {
        queue->tail->next = node;
    }
    queue->tail = node; // reset tail

    // signal all blocked threads to wake up as queue is now non-empty
    if (pthread_cond_broadcast(&(queue->cond)) != 0) {
        perror("pthread_cond_broadcast failed");
        exit(1);
    }

    if (pthread_mutex_unlock(&(queue->mutex)) != 0) {
        perror("pthread_mutex_unlock failed");
        exit(1);
    }
}

struct task* dequeue_task(struct task_queue* queue) {
    if (pthread_mutex_lock(&(queue->mutex)) != 0) {
        perror("pthread_mutex_lock() failed");
        exit(1);
    }

    // Block while queue is empty but open
    while (queue->head == NULL && !(queue->is_closed)) {
        if (pthread_cond_wait(&(queue->cond), &(queue->mutex)) != 0) {
            perror("pthread_cond_wait() failed");
            exit(1);
        }
    }

    // The queue is closed and empty
    if (queue->is_closed && queue->head == NULL) {
        if (pthread_mutex_unlock(&(queue->mutex)) != 0) {
            perror("pthread_mutex_unlock() failed");
            exit(1);
        }
        return NULL;
    }

    struct task_node* ret_node = queue->head;
    queue->head = queue->head->next; // remove and reset head
    
    // Set tail to NULL if queue is now empty
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    if (pthread_mutex_unlock(&(queue->mutex)) != 0) {
        perror("pthread_mutex_unlock failed");
        exit(1);
    }
    
    ret_node->next = NULL; // isolate node from queue
    struct task* ret = ret_node->tsk;
    free(ret_node);
    ret_node = NULL;

    return ret;
}