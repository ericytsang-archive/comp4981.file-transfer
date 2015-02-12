/**
 * header file for messagequeuehelper.c, exposing its interface.
 *
 * @sourceFile messagequeuehelper.h
 *
 * @program    server.out, client.out
 *
 * @function   void get_message_queue(int* msgQId);
 * @function   void make_message_queue(int* msgQId);
 * @function   void remove_message_queue(int msgQId);
 * @function   int msg_recv(int msgQId, Message* msg, int msgType);
 * @function   int msg_send(int msgQId, Message* msg, int msgType);
 * @function   int send_print_msg(int msgQId, void* str, int msgType);
 * @function   void msg_clear_type(int msgQId, int msgType);
 *
 * @date       2015-02-11
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 */
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
#define MSG_DATA_STOPCLNT 0
#define MSG_DATA_CONNECT  1
#define MSG_DATA_PRINT    2
#define MSG_DATA_DATA     3
#define MSG_DATA_PID      4

/**
 * payload of message sent to the server on the message queue, with message type
 *   1. it contains information about what the client, like its process id, what
 *   file it wants read to it, and with what priority client it is.
 */
typedef struct
{
    pid_t clientPid;
    int priority;
    char filePath[MAX_FILEPATH_LEN];
}
ConnectMsg;

/**
 * payload of the message sent to client processes. it contains a single string
 *   that the client should print to the screen.
 */
typedef struct
{
    char str[MAX_MSG_PRNTMSGSTR_LEN];
}
PrintMsg;

/**
 * payload of the message sent to the client process on the message queue. it
 *   contains data that needs to be print onto the client's screen. this is
 *   necessary because unlike printMsg, the data here may contain nulls.
 */
typedef struct
{
    int len;
    char data[MAX_MSG_DATAMSGDATA_LEN];
}
DataMsg;

/**
 * this is a message sent from the session process to the client on the message
 *   queue. it is used to inform the client of the session process's process id,
 *   so that when the client terminates, it can inform the session to cleanup
 *   and terminate as well.
 */
typedef struct
{
    pid_t pid;
}
PidMsg;

/**
 * payload of each message.
 */
typedef union
{
    ConnectMsg connectMsg;
    PrintMsg printMsg;
    DataMsg dataMsg;
    PidMsg pidMsg;
}
MsgData;

/**
 * the message structure that's passed around through the message queue.
 */
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
