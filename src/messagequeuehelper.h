#ifndef MESSAGEQUEUEHELPER_H
#define MESSAGEQUEUEHELPER_H

#include <stdbool.h>

/* message queue creation parameters */
#define MSGQ_KEY 8012
#define MSGQ_FLGS 0644 | IPC_CREAT

/* constant message types */
#define MSGQ_ACCEPT_T 1

/* message structures */
typedef struct
{
    long msgType;
    int payloadType;
    void* payload;
    int payloadLen;
}
Message;

typedef struct
{
    long msgType;
}
ConnectMsg;

typedef struct
{
    long msgType;
    long clntToSvrMsgType;
}
AcceptMsg;

/*SessionMsg*/

/* function prototypes */
int get_message_queue(int* msgQId);
int remove_message_queue(int msgQId)

#endif
