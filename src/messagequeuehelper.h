#ifndef MESSAGEQUEUEHELPER_H
#define MESSAGEQUEUEHELPER_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

/* message queue creation parameters */
#define MSGQ_KEY 8012

/* message constants */
#define MAX_MSG_PRNTMSGSTR_LEN 1024
#define MAX_MSG_DATAMSGDATA_LEN 255
#define MAX_FILEPATH_LEN 255

/* constant message types */
#define MSGQ_SVR_T    1
#define MSGQ_ACCEPT_T 2

/* constant message data types */
#define MSG_DATA_STOP_SVR 0         /* sent as type MSGQ_ACCEPT_T message. breaks out of the server loop */
#define MSG_DATA_CONNECT  1
#define MSG_DATA_PRINT    2
#define MSG_DATA_STOPCLNT 3
#define MSG_DATA_DATA     4
#define MSG_DATA_PID      5

/**
 * message data structures
 */
typedef struct
{
    pid_t clientPid;
    int priority;
    char filePath[MAX_FILEPATH_LEN];
}
ConnectMsg;

typedef struct
{
    char str[MAX_MSG_PRNTMSGSTR_LEN];
}
PrintMsg;

typedef struct
{
    int len;
    char data[MAX_MSG_DATAMSGDATA_LEN];
}
DataMsg;

typedef struct
{
    pid_t pid;
}
PidMsg;

/**
 * message structures
 */
typedef union
{
    ConnectMsg connectMsg;
    PrintMsg printMsg;
    DataMsg dataMsg;
    PidMsg pidMsg;
}
MsgData;

typedef struct
{
    long msgType;
    char dataType;
    MsgData data;
}
Message;

/**
 * function prototypes
 */
void get_message_queue(int* msgQId);
void make_message_queue(int* msgQId);
void remove_message_queue(int msgQId);
int msg_recv(int msgQId, Message* msg, int msgType);
int msg_send(int msgQId, Message* msg, int msgType);
int send_print_msg(int msgQId, void* str, int msgType);
void msg_clear_type(int msgQId, int msgType);

#endif
