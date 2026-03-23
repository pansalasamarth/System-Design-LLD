#ifndef MSG_STRUCT_H
#define MSG_STRUCT_H

#define MSG_SIZE 64

// Message structure (must start with long)
struct MyMsg {
    long mtype;              // message type (>0)
    char mtext[MSG_SIZE];    // message payload
};

#endif