#include <iostream>
#include <mqueue.h>
#include <fcntl.h>
#include <cstdlib>

using namespace std;

int main() {
    mqd_t mq = mq_open("/myqueue", O_RDONLY);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    struct mq_attr attr;
    mq_getattr(mq, &attr);

    char* buffer = (char*)malloc(attr.mq_msgsize);
    unsigned int prio;

    ssize_t bytes = mq_receive(mq, buffer, attr.mq_msgsize, &prio);
    if (bytes == -1) {
        perror("mq_receive");
        exit(1);
    }

    cout << "Received: " << buffer << " (priority " << prio << ")" << endl;

    free(buffer);
    mq_close(mq);
    mq_unlink("/myqueue");

    return 0;
}