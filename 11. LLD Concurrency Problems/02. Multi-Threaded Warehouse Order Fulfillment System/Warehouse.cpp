#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Warehouse.h"

extern order_queue_t queue;
extern warehouse_stats_t stats;
extern int total_expected_orders;

static int global_order_id = 1;

// Initialize Queue
void queue_init(order_queue_t *q)
{
    q->front = 0;
    q->rear = 0;
    q->count = 0;

    pthread_mutex_init(&q->mutex, NULL);
    sem_init(&q->sem_empty, 0, MAX_QUEUE);
    sem_init(&q->sem_full, 0, 0);
}

// Insert Order
void queue_insert(order_queue_t *q, order_t order)
{
    sem_wait(&q->sem_empty);

    pthread_mutex_lock(&q->mutex);

    q->orders[q->rear] = order;
    q->rear = (q->rear + 1) % MAX_QUEUE;
    q->count++;

    pthread_mutex_unlock(&q->mutex);

    sem_post(&q->sem_full);
}

// Remove Order
order_t queue_remove(order_queue_t *q)
{
    order_t order;

    sem_wait(&q->sem_full);

    pthread_mutex_lock(&q->mutex);

    order = q->orders[q->front];
    q->front = (q->front + 1) % MAX_QUEUE;
    q->count--;

    pthread_mutex_unlock(&q->mutex);

    sem_post(&q->sem_empty);

    return order;
}

// Initialize Statistics
void stats_init(warehouse_stats_t *s)
{
    s->total_shipped = 0;
    s->total_items_shipped = 0;

    for(int i = 0; i < NUM_PRIORITIES; i++)
    {
        s->shipped_by_priority[i] = 0;
    }

    pthread_mutex_init(&s->mutex, NULL);
    pthread_cond_init(&s->all_done, NULL);
}

// Print Priority
char* print_priority(int n)
{
    char* priority;
    if(n == 0)
    {
        priority = (char*)"LOW";
    }
    else if(n == 1)
    {
        priority = (char*)"MEDIUM";
    }
    else
    {
        priority = (char*)"HIGH";
    }
    return priority;
}

// Generator Thread
void* order_generator(void* arg)
{
    int id = *(int *)arg;

    for(int i = 0; i < ORDERS_PER_GENERATOR; i++)
    {
        order_t order;

        order.order_id = __sync_fetch_and_add(&global_order_id, 1);
        order.item_count = rand() % 10 + 1;
        order.priority = (priority_t)(rand() % 3);
        order.generated_by = id;

        printf("Generator %d created Order %d (%s, %d items)\n",
               order.generated_by,
               order.order_id,
               print_priority(order.priority),
               order.item_count);

        queue_insert(&queue, order);

        usleep((rand() % 200 + 100) * 1000);
    }
    return NULL;
}

// Picker Thread
void* picker(void* arg)
{
    int id = *(int *)arg;

    while(1)
    {
        order_t order = queue_remove(&queue);

        if(order.order_id == -1)
        {
            break;
        }

        printf("Picker %d shipped Order %d\n", id, order.order_id);

        usleep((rand() % 150 + 100) * 1000);

        pthread_mutex_lock(&stats.mutex);

        stats.total_shipped++;
        stats.total_items_shipped += order.item_count;
        stats.shipped_by_priority[order.priority]++;

        if(stats.total_shipped == total_expected_orders)
        {
            pthread_cond_signal(&stats.all_done);
        }

        pthread_mutex_unlock(&stats.mutex);
    }
    return NULL;
}