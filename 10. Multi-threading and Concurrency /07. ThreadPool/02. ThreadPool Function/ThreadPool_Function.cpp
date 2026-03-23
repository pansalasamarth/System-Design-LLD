#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

using namespace std;

#define THREAD_NUM 4

struct Task {
    void (*taskFunction)(int, int);
    int arg1, arg2;
};

// Task queue
Task taskQueue[256];
int taskCount = 0;

// Mutex and condition variable
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

// Mutex for printing to prevent interleaving
pthread_mutex_t coutMutex;

// Task functions
void sum(int a, int b) {
    usleep(50000); // simulate work
    int result = a + b;

    pthread_mutex_lock(&coutMutex);
    cout << "Sum of " << a << " and " << b << " is " << result << endl;
    pthread_mutex_unlock(&coutMutex);
}

void product(int a, int b) {
    usleep(50000); // simulate work
    int result = a * b;

    pthread_mutex_lock(&coutMutex);
    cout << "Product of " << a << " and " << b << " is " << result << endl;
    pthread_mutex_unlock(&coutMutex);
}

// Execute a task
void executeTask(Task* task) {
    task->taskFunction(task->arg1, task->arg2);
}

// Submit a task to the queue
void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

// Worker thread function
void* startThread(void* args) {
    while (true) {
        Task task;

        pthread_mutex_lock(&mutexQueue);
        while (taskCount == 0) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        // Pop the first task
        task = taskQueue[0];
        for (int i = 0; i < taskCount - 1; i++) {
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);

        executeTask(&task);
    }
    return nullptr;
}

int main() {
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    pthread_mutex_init(&coutMutex, NULL);

    // Create worker threads
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {
            perror("Failed to create the thread");
        }
    }

    srand(time(NULL));

    // Submit 100 tasks (alternating sum/product)
    for (int i = 0; i < 100; i++) {
        Task t = {
            .taskFunction = i % 2 == 0 ? &sum : &product,
            .arg1 = rand() % 100,
            .arg2 = rand() % 100
        };
        submitTask(t);
    }

    // Join threads (infinite loops, so this will block)
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(th[i], NULL);
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    pthread_mutex_destroy(&coutMutex);

    return 0;
}