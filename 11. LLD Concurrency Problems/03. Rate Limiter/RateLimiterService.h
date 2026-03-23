#ifndef RATELIMITER_SERVICE_H
#define RATELIMITER_SERVICE_H

#include "IRateLimiter.h"

class RateLimiterService {
private:
    IRateLimiter* limiter;

public:
    RateLimiterService(IRateLimiter* l) {
        limiter = l;
    }

    bool processRequest() {
        return limiter->allowRequest();
    }
};

#endif