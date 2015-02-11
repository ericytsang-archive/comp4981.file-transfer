#include "messagequeuehelper.h"

/**
 * gets a message queue from the operating system.
 *
 * @function   get_message_queue
 *
 * @date       2015-02-10
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * upon failure, the function exits the program.
 *
 * upon success, the function returns, and reassigns the value at the
 *   address of msgQId to the id of the message queue.
 *
 * @signature  int get_message_queue(int* msgQId)
 *
 * @param      msgQId pointer to integer that will hold the id of the message
 *   queue upon success.
 */
void get_message_queue(int* msgQId)
{
    int msggetResult = msgget((key_t) MSGQ_KEY, MSGQ_FLGS);
    if(msggetResult >= 0)
    {
        *msgQId = msggetResult;
        printf("msgQId: %d\n", *msgQId);
    }
    else
    {
        *msgQId = -1;
        fprintf(stderr, "get_message_queue failed: %d\n", errno);
        exit(1);
    }
}

/**
 * removes the identified message queue. returns upon success, exits otherwise.
 *
 * @function   remove_message_queue
 *
 * @date       2015-02-10
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  int remove_message_queue(int msgQId)
 *
 * @param      msgQId id number of the message queue to remove
 */
void remove_message_queue(int msgQId)
{
    if(msgctl(msgQId, IPC_RMID, 0) < 0)
    {
        fprintf(stderr, "remove_message_queue failed: %d\n", errno);
        exit(1);
    }
}

int msg_recv(int msgQId, Message* msg, int msgType)
{
    int msgLen = sizeof(Message);
    int returnValue = msgrcv(msgQId, msg, msgLen, msgType, 0);
    if(returnValue == -1)
    {
        fprintf(stderr, "msg_recv failed: %d\n", errno);
    }
    return returnValue;
}

int msg_send(int msgQId, Message* msg, int msgType)
{
    msg->msgType = msgType;
    return msgsnd(msgQId, msg, sizeof(Message), 0);
}
