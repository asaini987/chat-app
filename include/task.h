#ifndef __TASK
#define __TASK

#include <pthread.h>

/**
 * @brief Task representing the function and argument for a thread to execute.
 * 
 * @details Tasks encapsulate a function and its argument.
 * The thread pool will execute a task after it dequeues it from the task queue.
*/
struct task {
    void (*func) (void*); // thread function
    void* arg; // argument to thread function
};

/**
 * @brief Represents a node in the task queue.
*/
struct task_node {
    struct task* tsk; // the task containing the thread function info
    struct task_node* next; // pointer to the next node in the queue
};

/**
 * @brief Thread-safe queue to insert and delete tasks for the thread pool to execute.
 * 
 * @details This queue is implemented as a linked list with safe concurrent access.
 * It uses a mutex to ensure only one thread accesses the queue at a time 
 * and a condition varibale to signal when the queue is empty.
*/
struct task_queue {
    struct task_node* head; // head of the queue
    struct task_node* tail; // tail of the queue
    pthread_mutex_t mutex; // lock to synchronize threads
    pthread_cond_t cond; // signals threads to sleep when empty
};

/**
 * @brief Inserts a task to the end of the queue.
 * 
 * @param queue The pointer to the task queue
 * @param new_task The task to insert in the queue
*/
void enqueue_task(struct task_queue* queue, struct task* new_task);

/**
 * @brief Deletes a task from the front of the queue.
 * 
 * @param queue The pointer to the task queue.
 * 
 * @return The pointer to the former head of the queue.
*/
struct task* dequeue_task(struct task_queue* queue);

#endif