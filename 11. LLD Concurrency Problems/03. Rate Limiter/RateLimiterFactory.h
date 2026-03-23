#ifndef RATELIMITER_FACTORY_H
#define RATELIMITER_FACTORY_H

#include "TokenBucket.h"
#include "FixedWindow.h"

class RateLimiterFactory {
public:
    static IRateLimiter* createLimiter(int type) {
        if(type == 1)
            return new TokenBucket(5, 1);
        else
            return new FixedWindow(5);
    }
};

#endif