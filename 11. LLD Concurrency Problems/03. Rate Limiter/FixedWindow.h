#ifndef FIXED_WINDOW_H
#define FIXED_WINDOW_H

#include "IRateLimiter.h"
#include <pthread.h>
#include <ctime>

class FixedWindow : public IRateLimiter {
private:
    int max_requests;
    int count;
    time_t window_start;
    pthread_mutex_t mutex;

public:
    FixedWindow(int limit) {
        max_requests = limit;
        count = 0;
        window_start = time(NULL);
        pthread_mutex_init(&mutex, NULL);
    }

    bool allowRequest() override {
        pthread_mutex_lock(&mutex);

        time_t now = time(NULL);

        if(now - window_start >= 1) {
            count = 0;
            window_start = now;
        }

        if(count < max_requests) {
            count++;
            pthread_mutex_unlock(&mutex);
            return true;
        }

        pthread_mutex_unlock(&mutex);
        return false;
    }

    ~FixedWindow() {
        pthread_mutex_destroy(&mutex);
    }
};

#endif