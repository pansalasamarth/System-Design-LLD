#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "Warehouse.h"

order_queue_t queue;
warehouse_stats_t stats;
int total_expected_orders;

int main()
{
    pthread_t generators[NUM_GENERATORS];
    pthread_t pickers[NUM_PICKERS];

    int gid[NUM_GENERATORS];
    int pid[NUM_PICKERS];

    // Initialize queue and statistics structures
    queue_init(&queue);
    stats_init(&stats);

    // Create picker threads
    for(int i = 0; i < NUM_PICKERS; i++)
    {
        pid[i] = i;
        pthread_create(&pickers[i], NULL, picker, &pid[i]);
    }

    // Create generator threads
    for(int i = 0; i < NUM_GENERATORS; i++)
    {
        gid[i] = i;
        pthread_create(&generators[i], NULL, order_generator, &gid[i]);
    }

    // Join all generator threads
    for(int i = 0; i < NUM_GENERATORS; i++)
    {
        pthread_join(generators[i], NULL);
    }

    // Calculate total_expected_orders
    total_expected_orders = NUM_GENERATORS * ORDERS_PER_GENERATOR;

    // Wait until all orders are shipped
    pthread_mutex_lock(&stats.mutex);

    while(stats.total_shipped < total_expected_orders)
    {
        pthread_cond_wait(&stats.all_done, &stats.mutex);
    }

    pthread_mutex_unlock(&stats.mutex);

    // Trigger picker shutdown (poison pills)
    for(int i = 0; i < NUM_PICKERS; i++)
    {
        order_t poison;
        poison.order_id = -1;
        queue_insert(&queue, poison);
    }

    // Join all picker threads
    for(int i = 0; i < NUM_PICKERS; i++)
    {
        pthread_join(pickers[i], NULL);
    }

    // Print final report
    printf("===== FINAL REPORT =====\n");
    printf("Total Orders Shipped: %d\n", stats.total_shipped);
    printf("Total Items Shipped: %d\n", stats.total_items_shipped);
    printf("HIGH: %d\n", stats.shipped_by_priority[HIGH]);
    printf("MEDIUM: %d\n", stats.shipped_by_priority[MEDIUM]);
    printf("LOW: %d\n", stats.shipped_by_priority[LOW]);
    printf("========================\n");

    // Destroy all synchronization primitives
    pthread_mutex_destroy(&queue.mutex);
    pthread_mutex_destroy(&stats.mutex);
    pthread_cond_destroy(&stats.all_done);
    sem_destroy(&queue.sem_empty);
    sem_destroy(&queue.sem_full);

    return 0;
}