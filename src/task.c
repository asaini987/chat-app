#include <stdlib.h>
#include <stdio.h>

#include "task.h"

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
    if (pthread_cond_broadcast(&queue->cond) != 0) {
        perror("pthread_cond_broadcast() failed");
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

    // Block while queue is empty and the queue is open
    while (queue->head == NULL && !queue->is_closed) {
        if (pthread_cond_wait(&queue->cond, &queue->mutex) != 0) {
            perror("pthread_cond_wait() failed");
            exit(1);
        }
    }

    // The queue will not take any more tasks and it is empty
    if (queue->is_closed && queue->head == NULL) {
        pthread_mutex_unlock(&queue->mutex);
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