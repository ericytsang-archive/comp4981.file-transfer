/**
 * server source file.
 *
 * @sourceFile server.c
 *
 * @program    server.out
 *
 * @function   int main(void)
 * @function   static int sigint_handler(int sigNum)
 * @function   static void msgq_read_loop(int msgQId)
 * @function   static bool parse_msgq_msg(Message* msg)
 * @function   static void handle_connect_msg(ConnectMsg* connectMsg)
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
 * the server waits for clients to connect, and parses their request, and
 *   transfers the files contents to the server.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include "messagequeuehelper.h"
#include "session.h"

/* typedefs */
typedef void (*sighandler_t)(int);

/* function prototypes */
static void sigint_handler(int);
static int msgq_read_loop(int);
static bool parse_msgq_msg(Message*);
static void handle_connect_msg(ConnectMsg*);

/**
 * message queue id used by the server.
 */
static int msgQId;
static sighandler_t previousSigHandler;

/**
 * sets up the message queue, and listens for clients to connect.
 *
 * @function   main
 *
 * @date       2015-02-10
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  int main(void)
 *
 * @return     return code, indication the nature of process termination.
 */
int main(void)
{
    int exitCode;

    /* create the message queue. */
    make_message_queue(&msgQId);

    /* set up signal handler to remove IPC. */
    previousSigHandler = signal(SIGINT, sigint_handler);

    /* execute main loop of the server. */
    exitCode = msgq_read_loop(msgQId);

    /* remove message queue. */
    remove_message_queue(msgQId);

    /* end program... */
    return exitCode;
}

/**
 * signal enqueues a stop server message into the server's message queue.
 *
 * @function   sigint_handler
 *
 * @date       2015-02-10
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note
 *
 * signal enqueues a stop server message into the server's message queue, so
 *   that when the server parses the message, it will break out of the server
 *   loop, and end normally.
 *
 * @signature  static int sigint_handler(int sigNum)
 *
 * @param      sigNum type of signal received
 */
static void sigint_handler(int sigNum)
{
    remove_message_queue(msgQId);
    exit(sigNum);
}

/**
 * blocking function. this is the loop that reads from the message queue, and
 *   passes them on to handler functions.
 *
 * @function   msgq_read_loop
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
 * @signature  static void msgq_read_loop(int msgQId)
 *
 * @param      msgQId [description]
 *
 * @return     0 upon normal loop exit (EOF); 1 otherwise (error).
 */
static int msgq_read_loop(int msgQId)
{
    Message msg;
    static int breakMsgLoop = 0;

    while(!breakMsgLoop)
    {
        switch(msg_recv(msgQId, &msg, MSGQ_SVR_T))
        {
        case 0:
            breakMsgLoop = true;
            break;
        case -1:
            breakMsgLoop = true;
            break;
        default:
            breakMsgLoop = !parse_msgq_msg(&msg);
            break;
        }
    }

    return 0;
}

/**
 * parses and handles the passed message.
 *
 * @function   parse_msgq_msg
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
 * returns true upon success, and the message dequeueing process should
 *   continue; false otherwise.
 *
 * @signature  static bool parse_msgq_msg(Message* msg)
 *
 * @param      msg pointer to the message to parse.
 *
 * @return     true upon success, and the message dequeueing process should
 *   continue; false otherwise.
 */
static bool parse_msgq_msg(Message* msg)
{
    int returnVal;

    switch(msg->dataType)
    {
    case MSG_DATA_STOP_SVR:
        returnVal = false;
        break;
    case MSG_DATA_CONNECT:
        handle_connect_msg(&msg->data.connectMsg);
        returnVal = true;
        break;
    }

    return returnVal;
}

/**
 * handles the connection request message.
 *
 * @function   handle_connect_msg
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
 * this function handles a connection request message by starting a new process
 *   that will be used to serve the client.
 *
 * @signature  static void handle_connect_msg(ConnectMsg* connectMsg)
 *
 * @param      connectMsg pointer to the received ConnectMsg structure
 */
static void handle_connect_msg(ConnectMsg* connectMsg)
{
    /* create a new process to serve the client */
    if(fork() == 0)
    {
        int returnValue;

        /* reset signal handler */
        signal(SIGINT, previousSigHandler);

        /* print connection request */
        printf("connectMsg:\n");
        printf("    clientPid: %d\n", connectMsg->clientPid);
        printf("    priority: %d\n", connectMsg->priority);
        printf("    filePath: %s\n", connectMsg->filePath);

        /* handle connection request */
        returnValue = serve_client(
            connectMsg->clientPid,
            connectMsg->priority,
            connectMsg->filePath);

        exit(returnValue);
    }
}
