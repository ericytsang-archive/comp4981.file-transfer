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
        msgQId = msggetResult;
    }
    else
    {
        msgQId = -1;
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

int message_queue_recv_accept(int msgQId, void* acceptStruct)
{
    return msgrcv(msgQId, acceptStruct, sizeof(AcceptMsg), MSGQ_ACCEPT_T, 0);
}

static int message_queue_recv(int msgQId, void* buffer, int bytesToRead, int msgType)
{
    return msgrcv(msgQId, buffer, bytesToRead, msgType, 0);
}

int message_queue_send()
{
    return 0;
}

int child(int msgQId)
{
    Message msg;
    size_t bytesRead = 0;
    bytesRead = msgrcv(msgQId, &msg, sizeof(Message), 1, 0);
    printf("%d : %s : %d\n", (int) bytesRead, msg.text, errno);
    bytesRead = msgrcv(msgQId, &msg, sizeof(Message), 1, 0);
    printf("%d : %s : %d\n", (int) bytesRead, msg.text, errno);
    bytesRead = msgrcv(msgQId, &msg, sizeof(Message), 1, 0);
    printf("%d : %s : %d\n", (int) bytesRead, msg.text, errno);
    bytesRead = msgrcv(msgQId, &msg, sizeof(Message), 1, 0);
    printf("%d : %s : %d\n", (int) bytesRead, msg.text, errno);
    return 0;
}

int parent(int msgQId)
{
    /* sending message */
    Message msg;
    msg.msgType = 1;
    msg.text = "hey there";
    msgsnd(msgQId, &msg, sizeof(Message), 1);
    sleep(1);
    msgsnd(msgQId, &msg, sizeof(Message), 1);
    sleep(1);
    msgsnd(msgQId, &msg, sizeof(Message), 1);
    sleep(1);

    /* Remove the message queue */
    if (msgctl (msgQId, IPC_RMID, 0) < 0)
    {
        perror ("msgctl (remove queue) failed!");
        exit (3);
    }

    return 0;
}
