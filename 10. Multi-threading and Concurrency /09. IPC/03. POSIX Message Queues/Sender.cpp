#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>

using namespace std;

int main() {
    mqd_t mq = mq_open("/myqueue", O_CREAT | O_WRONLY, 0666, NULL);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    const char* msg = "Hello from sender";
    if (mq_send(mq, msg, strlen(msg) + 1, 1) == -1) {
        perror("mq_send");
        exit(1);
    }

    mq_close(mq);
    return 0;
}