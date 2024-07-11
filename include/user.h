#ifndef __USER
#define __USER

#include "message.h"

/**
 * @brief Represents a user in the chat room.
*/
struct user {
    int connfd; // socket file descriptor associated with the user
    char* username; // the username of the user in the chat room
    struct message_queue* msg_queue; // a thread-safe queue of incoming messages for this user
};

/**
 * @brief Represents a node in the linked list of users.
*/
struct user_node {
    struct user* usr; // the user structure associated with the node
    struct user_node* next; // the next node in the linked list
};

/**
 * @brief Represents a bucket in the thread-safe user_map.
*/
struct user_bucket {
    struct user_node* head; // The linked list of user nodes
    pthread_rwlock_t rw_lock; // a read-write lock for the bucket
};

/**
 * @brief Thread-safe hash map to store their corresponding user structs.
 * Key-Value Pair: <char* username, struct user* usr>.
 * 
 * @details Utilizes chaining to handle collisions. Each bucket is locked with a read-write lock
 * to facilitate simultaneous reads, but exclusive writes. A mutex is used to guard the size, capacity,
 * and load factor fields.
*/
struct user_map {
    struct user_bucket** table; // the hash table storing the key-value
    int size; // the amount of users in the hash map
    int capacity; // the length of the hash table array
    float load_factor; // ratio = size / capacity
    pthread_mutex_t mutex; // mutex to guard the size and load factor fields of the map
};

/**
 * @brief Creates a thread-safe hash map to store users and their usernames.
 * 
 * @details Initializes a hash map with 100 buckets at first. The internal hash table
 * grows dynamically as needed.
 * 
 * @return A pointer to the dynamically allocated user map.
*/
struct user_map* user_map_init();

/**
 * @brief Destroys the user map structure.
 * 
 * @param usr_map A pointer to the user hash map to destroy.
 * 
 * @details Frees the entire hash table and destroys all synchronization structures.
*/
void user_map_destroy(struct user_map* usr_map);

/**
 * @brief Attempts to add a username and user pair to the hash map. No duplicate usernames are added.
 * 
 * @param map A pointer to the user hash map to modify
 * @param username The username of the user to store as the key
 * @param usr The user struct to store as the value
 * 
 * @return 0 on a successful add, -1 if a username already exists in the map.
*/
int add_user(struct user_map* map, char* username, struct user* usr);

/**
 * @brief Attempts to remove a user from the hash map.
 * 
 * @param map A pointer to the user hash map to modify
 * @param username The username of the user to remove
 * 
 * @return A pointer to the user that was removed, NULL if the user was not in the map.
*/
struct user* remove_user(struct user_map* map, char* username);

/**
 * @brief Checks if a user with the username is already in the hash map
 * 
 * @param map A pointer to the hash map to search.
 * @param username The username that will be checked for duplicates within the map.
 * 
 * @return 1 if there is a duplicate username, 0 if not.
*/
int is_duplicate_username(struct user_map* map, char* username);

/**
 * @brief Attempts to retrieve a user from the hash table.
 * 
 * @param map A pointer to the user hash map to search
 * @param username The username of the user to retrieve
 * 
 * @return The pointer to the user struct if it is in the map, NULL if not.
*/
struct user* get_user(struct user_map* map, char* username);

#endif