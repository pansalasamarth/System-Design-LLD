#ifndef IRATELIMITER_H
#define IRATELIMITER_H

class IRateLimiter {
public:
    virtual bool allowRequest() = 0;
    virtual ~IRateLimiter() {}
};

#endif