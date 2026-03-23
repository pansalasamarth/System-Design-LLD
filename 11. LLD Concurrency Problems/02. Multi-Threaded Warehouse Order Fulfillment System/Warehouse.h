#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <pthread.h>
#include <semaphore.h>

#define MAX_QUEUE 10
#define NUM_GENERATORS 3
#define NUM_PICKERS 3
#define ORDERS_PER_GENERATOR 50
#define NUM_PRIORITIES 3

// enum for order priority
enum priority_t {
    LOW = 0,
    MEDIUM,
    HIGH
};

// Order Structure
struct order_t {
    int order_id;        
    int item_count;      
    priority_t priority; 
    int generated_by;    
};

// Queue Structure
struct order_queue_t {
    order_t orders[MAX_QUEUE];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    sem_t sem_empty;  // initialized to MAX_QUEUE
    sem_t sem_full;   // initialized to 0
};

// Statistics
struct warehouse_stats_t {
    int total_shipped;
    int total_items_shipped;
    int shipped_by_priority[NUM_PRIORITIES];
    pthread_mutex_t mutex;
    pthread_cond_t  all_done;
};

// Function Declarations
void queue_init(order_queue_t *q);
void queue_insert(order_queue_t *q, order_t order);
order_t queue_remove(order_queue_t *q);

void stats_init(warehouse_stats_t *s);

void* order_generator(void* arg);
void* picker(void* arg);

#endif