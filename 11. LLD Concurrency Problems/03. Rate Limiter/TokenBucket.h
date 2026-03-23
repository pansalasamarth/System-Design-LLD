#ifndef TOKEN_BUCKET_H
#define TOKEN_BUCKET_H

#include "IRateLimiter.h"
#include <pthread.h>
#include <ctime>
#include <algorithm>

class TokenBucket : public IRateLimiter {
private:
    int capacity;
    int tokens;
    int refill_rate;
    time_t last_refill;
    pthread_mutex_t mutex;

public:
    TokenBucket(int cap, int rate) {
        capacity = cap;
        tokens = cap;
        refill_rate = rate;
        last_refill = time(NULL);
        pthread_mutex_init(&mutex, NULL);
    }

    bool allowRequest() override {
        pthread_mutex_lock(&mutex);

        time_t now = time(NULL);
        int elapsed = now - last_refill;

        tokens = std::min(capacity, tokens + elapsed * refill_rate);
        last_refill = now;

        if(tokens > 0) {
            tokens--;
            pthread_mutex_unlock(&mutex);
            return true;
        }

        pthread_mutex_unlock(&mutex);
        return false;
    }

    ~TokenBucket() {
        pthread_mutex_destroy(&mutex);
    }
};

#endif