#include <iostream>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include "Msg_struct.h"

using namespace std;

int main() {
    // Create unique key (file must exist)
    key_t key = ftok("progfile", 65);
    if (key == -1) { perror("ftok"); exit(1); }

    // Create or get message queue
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) { perror("msgget"); exit(1); }

    // Prepare message
    MyMsg msg;
    msg.mtype = 1;

    snprintf(msg.mtext, MSG_SIZE, "Hello from producer!");

    // Send message
    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    cout << "Producer: message sent." << endl;

    return 0;
}