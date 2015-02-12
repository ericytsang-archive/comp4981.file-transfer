/**
 * this file contains helper and wrapper functions used to interface with
 *   message queues.
 *
 * @sourceFile messagequeuehelper.c
 *
 * @program    server.out, client.out
 *
 * @function   void make_message_queue(int* msgQId)
 * @function   int get_message_queue(int* msgQId)
 * @function   int remove_message_queue(int msgQId)
 * @function   int msg_recv(int msgQId, Message* msg, int msgType)
 * @function   int msg_send(int msgQId, Message* msg, int msgType)
 * @function   void msg_clear_type(int msgQId, int msgType)
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
#include "messagequeuehelper.h"

/**
 * gets a new message queue from the operating system.
 *
 * @function   make_message_queue
 *
 * @date       2015-02-11
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note
 *
 * upon failure, the function exits the process, and prints an error message to
 *   the screen.
 *
 * upon success, the value at the address of msgQId is set to the id of the
 *   message queue.
 *
 * @signature  void make_message_queue(int* msgQId)
 *
 * @param      msgQId pointer to an integer that will be assigned the id of the
 *   new message queue.
 */
void make_message_queue(int* msgQId)
{
    int msggetResult = msgget((key_t) MSGQ_KEY, 0644 | IPC_CREAT | IPC_EXCL);
    if(msggetResult >= 0)
    {
        *msgQId = msggetResult;
    }
    else
    {
        *msgQId = -1;
        fprintf(stderr, "make_message_queue failed: %d\n", errno);
        exit(1);
    }
}

/**
 * gets an existing message queue from the operating system.
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
 * upon failure, the function exits the program and prints a fail message.
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
    int msggetResult = msgget((key_t) MSGQ_KEY, 0);
    if(msggetResult >= 0)
    {
        *msgQId = msggetResult;
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

/**
 * reads a message from the message queue into the passed message pointer.
 *
 * @function   msg_recv
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
 *
 * @signature  int msg_recv(int msgQId, Message* msg, int msgType)
 *
 * @param      msgQId id of the message queue to write messages to
 * @param      msg pointer to a Message structure to write the read message into
 * @param      msgType type of message to read from the message queue
 *
 * @return     number of bytes read from the message queue, -1; if an error
 *   occurs
 */
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

/**
 * writes the referenced message to the message queue.
 *
 * @function   msg_send
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
 *
 * @signature  int msg_send(int msgQId, Message* msg, int msgType)
 *
 * @param      msgQId id of the message queue to send the message to.
 * @param      msg pointer to a message structure to write to the message queue.
 * @param      msgType type of the message.
 *
 * @return     0 if the message was sent successfully; false otherwise.
 */
int msg_send(int msgQId, Message* msg, int msgType)
{
    msg->msgType = msgType;
    return msgsnd(msgQId, msg, sizeof(Message), 0);
}

/**
 * clears all messages of the passed type from the identified message queue.
 *
 * @function   msg_clear_type
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
 *
 * @signature  void msg_clear_type(int msgQId, int msgType)
 *
 * @param      msgQId id of the message queue to clear
 * @param      msgType type of message to clear from the message queue
 */
void msg_clear_type(int msgQId, int msgType)
{
    Message msg;
    int msgLen = sizeof(Message);

    /* read messages from the message queue, until they're all gone */
    while(msgrcv(msgQId, &msg, msgLen, msgType, 0) > 0);
}
