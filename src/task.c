#include <stdlib.h>

#include "task.h"

/*
* ADD THREAD SAFETY SYNC TO ALL FUNCS
*/

void enqueue_task(struct task_queue* queue, struct task* new_task) {
    struct task_node* node = (struct task_node*) malloc(sizeof(struct task_node));
    node->tsk = new_task;
    node->next = NULL;

    pthread_mutex_lock(&(queue->mutex));
    if (queue->tail == NULL) { // if adding as first element
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    pthread_mutex_unlock(&(queue->mutex));
}

struct task* dequeue_task(struct task_queue* queue) {
    pthread_mutex_lock(&(queue->mutex));

    if (queue->head == NULL) { // empty
        return NULL;
    }

    struct task_node* ret_node = queue->head;
    queue->head = queue->head->next; // remove and reset head

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    struct task* ret = ret_node->tsk;
    free(ret_node);

    pthread_mutex_unlock(&(queue->mutex));

    return ret;
}