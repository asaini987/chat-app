#include <stdlib.h>

#include "task.h"

/*
* ADD THREAD SAFETY SYNC TO ALL FUNCS
*/

void enqueue_task(struct TaskQueue* queue, struct Task* task) {
    struct TaskNode* node = (struct TaskNode*) malloc(sizeof(struct TaskNode));
    node->task = task;
    node->next = NULL;

    pthread_mutex_lock(&(queue->mutex));
    if (queue->tail == NULL) { // first element
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    pthread_mutex_unlock(&(queue->mutex));
}

struct Task* dequeue_task(struct TaskQueue* queue) {
    pthread_mutex_lock(&(queue->mutex));

    if (queue->head == NULL) { // empty
        return NULL;
    }

    struct TaskNode* ret_node = queue->head;
    queue->head = queue->head->next; // remove and reset head

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    struct Task* ret = ret_node->task;
    free(ret_node);

    pthread_mutex_unlock(&(queue->mutex));

    return ret;
}   