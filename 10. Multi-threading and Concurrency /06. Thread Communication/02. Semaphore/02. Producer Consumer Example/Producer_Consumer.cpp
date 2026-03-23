#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <semaphore.h>

using namespace std;

#define THREAD_NUM 8

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

int buffer[10];
int count = 0;

void* producer(void* args) {
    while (1) {
        // Produce
        int x = rand() % 100;
        sleep(1);

        // Add to buffer
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);

        buffer[count] = x;
        count++;

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
    }
    return nullptr;
}

void* consumer(void* args) {
    while (1) {
        int y;

        // Remove from buffer
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);

        y = buffer[count - 1];
        count--;

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);

        // Consume
        cout << "Got " << y << endl;
        sleep(1);
    }
    return nullptr;
}

int main() {
    srand(time(NULL));

    pthread_t th[THREAD_NUM];

    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);

    for (int i = 0; i < THREAD_NUM; i++) {
        if (i == 0) {
            if (pthread_create(&th[i], NULL, &consumer, NULL) != 0) {
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &producer, NULL) != 0) {
                perror("Failed to create thread");
            }
        }
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);

    return 0;
}