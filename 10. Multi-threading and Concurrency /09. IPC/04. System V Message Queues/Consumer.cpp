#include <iostream>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "Msg_struct.h"

using namespace std;

int main() {
    // Same key as producer
    key_t key = ftok("progfile", 65);
    if (key == -1) { perror("ftok"); exit(1); }

    // Get existing queue
    int msgid = msgget(key, 0666);
    if (msgid == -1) { perror("msgget"); exit(1); }

    // Receive message
    MyMsg msg;
    if (msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    cout << "Consumer: received message: " << msg.mtext << endl;

    // Remove queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    return 0;
}