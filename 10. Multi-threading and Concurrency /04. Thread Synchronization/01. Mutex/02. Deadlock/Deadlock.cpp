#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>

using namespace std;

#define MAX_THREAD 8

int fuel = 40;
pthread_mutex_t mutexFuel;

int water = 10;
pthread_mutex_t mutexWater;

void* routine(void* args) {
    if(rand() % 2 == 1)
    {
        pthread_mutex_lock(&mutexFuel);   // acquire by thread 1
        sleep(1);
        pthread_mutex_lock(&mutexWater);  // may wait (deadlock scenario)
    }
    else
    {
        pthread_mutex_lock(&mutexWater);  // acquire by thread 2
        sleep(1);
        pthread_mutex_lock(&mutexFuel);   // may wait (deadlock scenario)
    }

    fuel += 40;
    water = fuel;

    cout << "fuel " << fuel << ", water " << water << endl;

    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexWater);

    return nullptr;
}

int main()
{
    srand(time(NULL));

    pthread_t t[MAX_THREAD];

    pthread_mutex_init(&mutexFuel, NULL);
    pthread_mutex_init(&mutexWater, NULL);

    for(int i = 0; i < MAX_THREAD; i++)
    {
        pthread_create(&t[i], NULL, &routine, NULL);
    }

    for(int i = 0; i < MAX_THREAD; i++)
    {
        pthread_join(t[i], NULL);
    }

    cout << "total fuel " << fuel << ", water " << water << endl;

    pthread_mutex_destroy(&mutexFuel);
    pthread_mutex_destroy(&mutexWater);

    return 0;
}