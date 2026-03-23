#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <semaphore.h>

using namespace std;

sem_t semaphore;

void* routine(void* arg) {
    sem_wait(&semaphore); // only 2 threads allowed at a time

    sleep(1);
    cout << "hello from thread " << *(int*)arg << endl;

    sem_post(&semaphore);

    free(arg);
    return nullptr;
}

int main()
{
    pthread_t t[5];

    sem_init(&semaphore, 0, 2);

    for(int i = 0; i < 5; i++)
    {
        int* a = (int*)malloc(sizeof(int));
        *a = i;

        pthread_create(&t[i], NULL, &routine, a);
    }

    for(int i = 0; i < 5; i++)
    {
        pthread_join(t[i], NULL);
    }

    sem_destroy(&semaphore);

    return 0;
}