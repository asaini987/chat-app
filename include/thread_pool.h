#ifndef __THREAD_POOL
#define __THREAD_POOL

#include "task.h"

/**
 * @brief A pool of threads that execute tasks as they are added to their shared task queue.
 * 
 * @details Minimizes overhead of repeated thread creation and destruction.
 * Allows for the use of "reusable" threads and does not restrict the server 
 * to a one thread-per-client model.
*/
struct thread_pool {
    pthread_t* threads; // array of threads
    int thread_cnt; // number of threads in the pool
    struct task_queue* tsk_queue; // thread-safe queue for tasks
};

/**
 * @brief Initializes a new thread pool containing a thread-safe task queue.
 * 
 * @param num_threads The number of threads to create in the thread pool.
 * 
 * @return The pointer to the newly created thread pool.
*/
struct thread_pool* thread_pool_init(int num_threads);

/**
 * @brief Destroys the thread pool structure.
 * 
 * @details Reaps all threads, destroys synchronization constructs, and frees the task queue.
 * 
 * @param tpool The pointer to the thread pool to destroy.
*/
void thread_pool_destroy(struct thread_pool* tpool);

#endif