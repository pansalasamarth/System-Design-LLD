#include <iostream>
#include <unistd.h>
#include "RateLimiterFactory.h"
#include "RateLimiterService.h"

using namespace std;

int main() {
    IRateLimiter* limiter = RateLimiterFactory::createLimiter(1);
    RateLimiterService service(limiter);

    for(int i = 0; i < 20; i++) {
        if(service.processRequest())
            cout << "Request Allowed\n";
        else
            cout << "Request Blocked\n";

        usleep(200000); // 0.2 sec
    }

    delete limiter;
    return 0;
}