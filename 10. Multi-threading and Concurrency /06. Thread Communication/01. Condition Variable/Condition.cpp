#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

int items = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void* produce(void* args) {
    for(int i = 0; i < 30; i++)
    {
        pthread_mutex_lock(&mutex);

        items++;
        cout << "producer: " << items << endl;

        pthread_cond_broadcast(&cond);

        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return nullptr;
}

void* consume(void* args) {
    for(int i = 0; i < 30; i++)
    {
        pthread_mutex_lock(&mutex);

        while(items == 0)
        {
            cout << "waiting consumer.." << endl;
            pthread_cond_wait(&cond, &mutex);
        }

        items--;
        cout << "consumer: " << items << endl;

        pthread_mutex_unlock(&mutex);
        sleep(5);
    }
    return nullptr;
}

int main()
{
    pthread_t t[4];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for(int i = 0; i < 4; i++)
    {
        if(i == 0)
        {
            pthread_create(&t[i], NULL, &produce, NULL);
        }
        else
        {
            pthread_create(&t[i], NULL, &consume, NULL);
        }
    }

    for(int i = 0; i < 4; i++)
    {
        pthread_join(t[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}