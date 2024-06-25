#include <stdlib.h>

#include "task.h"

/*
* ADD THREAD SAFETY SYNC TO ALL FUNCS
*/

void enqueue_task(struct task_queue* queue, struct task* new_task) {
    struct task_node* node = NULL;
    node = (struct task_node*) malloc(sizeof(struct task_node));

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

    if (queue->tail == NULL) { // if adding as first element, add as head
        queue->head = node;
        queue->tail = node; // set tail to one and only element
    } else { // else add onto tail
        queue->tail->next = node;
        queue->tail = node; // reset tail to new element
    }

    if (pthread_mutex_unlock(&(queue->mutex)) != 0) {
        perror("pthread_mutex_unlock failed");
        exit(1);
    };
}

struct task* dequeue_task(struct task_queue* queue) {
    if (pthread_mutex_lock(&(queue->mutex)) != 0) {
        perror("pthread_mutex_lock() failed");
        exit(1);
    }

    // Attempted to dequeue an empty queue
    if (queue->head == NULL) { 
        return NULL;
    }

    struct task_node* ret_node = queue->head;
    queue->head = queue->head->next; // remove and reset head

    // Set tail to NULL if queue is now empty
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    struct task* ret = ret_node->tsk;
    free(ret_node);

    if (pthread_mutex_unlock(&(queue->mutex)) != 0) {
        perror("pthread_mutex_unlock failed");
        exit(1);
    };

    return ret;
}