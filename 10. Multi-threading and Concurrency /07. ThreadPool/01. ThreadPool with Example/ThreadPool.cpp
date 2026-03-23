#include <iostream>
#include <pthread.h>
#include <queue>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

using namespace std;

#define THREAD_NUM 4

struct Task {
    int a, b;
};

queue<Task> taskQueue;
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

// Mutex for printing to avoid interleaved output
pthread_mutex_t coutMutex;

void executeTask(Task* task) {
    usleep(50000); // simulate work
    int result = task->a + task->b;

    // Lock cout to prevent interleaving
    pthread_mutex_lock(&coutMutex);
    cout << "The sum of " << task->a << " and " << task->b << " is " << result << endl;
    pthread_mutex_unlock(&coutMutex);
}

// Submit a task to the queue
void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue.push(task);
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

// Worker thread function
void* startThread(void* args) {
    while (true) {
        Task task;

        pthread_mutex_lock(&mutexQueue);
        while (taskQueue.empty()) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }
        task = taskQueue.front();
        taskQueue.pop();
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

    // Submit 100 tasks
    for (int i = 0; i < 100; i++) {
        Task t = { rand() % 100, rand() % 100 };
        submitTask(t);
    }

    // Join threads (in this example threads run infinitely)
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(th[i], NULL);
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    pthread_mutex_destroy(&coutMutex);

    return 0;
}