/**
 * the client program.
 *
 * @sourceFile client.c
 *
 * @program    client.out
 *
 * @function   int main (int argc , char** argv)
 * @function   static void msgq_loop(int msgQId)
 * @function   static void connect(int msgQId, int priority, char* filePath)
 * @function   static void sigint_handler(int sigNum)
 * @function   static void* exit_on_char(void* nothing)
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
 * the client program connects to the server, and requests a file to be sent to
 *   it through the message queue, and then reads the file contents from the
 *   message queue, and prints it to the screen.
 */
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "messagequeuehelper.h"
#include "stdbool.h"

/* function prototypes */
static void msgq_loop(int msgQId);
static void connect(int msgQId, int priority, char* filePath);
static void sigint_handler(int sigNum);
static void* exit_on_char(void* nothing);

/* inter process communication globals */
static int msgQId;
static int sessionPid = 0;

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
 * @signature  int main(int argc , char** argv)
 *
 * @param      argc number of command line arguments, including the program
 *   name.
 * @param      argv array of c-style character arrays that are the command line
 *   arguments.
 *
 * @return     return code, indication the nature of process termination.
 */
int main(int argc , char** argv)
{
    pthread_t exitOnCharThread;

    /* verify command line arguments */
    if(argc != 3)
    {
        printf("usage: %s [priority] [filepath]\n", argv[0]);
        exit(0);
    }

    /* set signal handler */
    signal(SIGINT, sigint_handler);

    /* start exit on character thread */
    pthread_create(&exitOnCharThread, NULL, exit_on_char, 0);

    /* get the message queue. */
    get_message_queue(&msgQId);

    /* send connection message to server */
    connect(msgQId, atoi(argv[1]), argv[2]);

    /* get messages from server until stop */
    msgq_loop(msgQId);

    /* end program... */
    return 0;
}

/**
 * sends a connect message to the server.
 *
 * @function   connect
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
 * @signature  static void connect(int msgQId, int priority, char* filePath)
 *
 * @param      msgQId id of the message queue to send the connect message to.
 * @param      priority priority of this client. the higher the priority, the
 *   faster it will get its messages. highest priority is 0, lowest priority is
 *   20.
 * @param      filePath path to file to have sent to the client through the
 *   message queue.
 */
static void connect(int msgQId, int priority, char* filePath)
{
    /* construct connect message */
    Message msg;
    msg.dataType = MSG_DATA_CONNECT;
    msg.data.connectMsg.clientPid   = getpid();
    msg.data.connectMsg.priority    = priority;
    strncpy(msg.data.connectMsg.filePath, filePath, strlen(filePath)+1);

    /* send connection message to server */
    msg_send(msgQId, &msg, MSGQ_SVR_T);
}

/**
 * message loop of the client, it continuously dequeues messages from the
 *   message queue, and processes them.
 *
 * @function   msgq_loop
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
 * @signature  static void msgq_loop(int msgQId)
 *
 * @param      msgQId id of the message queue to dequeue from.
 */
static void msgq_loop(int msgQId)
{
    static bool stopLoop = false;

    while(!stopLoop)
    {
        Message msg;
        if(msg_recv(msgQId, &msg, getpid()) < 0)
        {
            stopLoop = true;
        }
        switch(msg.dataType)
        {
        case MSG_DATA_DATA:
            printf("%.*s", msg.data.dataMsg.len, msg.data.dataMsg.data);
            break;
        case MSG_DATA_PRINT:
            printf("%s", msg.data.printMsg.str);
            break;
        case MSG_DATA_STOPCLNT:
            stopLoop = true;
            break;
        case MSG_DATA_PID:
            sessionPid = msg.data.pidMsg.pid;
            break;
        default:
            fprintf(stderr, "unknown message type!\n");
            kill(sessionPid, SIGUSR1);
            stopLoop = true;
            break;
        }
    }
}

/**
 * interrupt handler for the client.
 *
 * @function   sigint_handler
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
 * the interrupt handler for the client. it sends a signal to its server process
 *   telling it that the client is terminating. this sets the session perform
 *   cleanup, and stop writing to the message queue.
 *
 * @signature  static void sigint_handler(int sigNum)
 *
 * @param      sigNum signal number that is invoking this function. this will
 *   always be SIGINT.
 */
static void sigint_handler(int sigNum)
{
    /* send signal to session indicating to them that we are no longer */
    kill(sessionPid, SIGUSR1);

    /* exit... */
    exit(sigNum);
}

/**
 * threaded function. it makes the process exit when a character is received
 *   from stdin.
 *
 * @function   exit_on_char
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
 * @signature  static void* exit_on_char(void* nothing)
 *
 * @param      nothing pointer to address 0
 */
static void* exit_on_char(void* nothing)
{
    /* wait for input */
    getchar();

    /* kill ourself */
    kill(getpid(), SIGINT);

    /* exit thread */
    pthread_exit(0);
    return nothing;
}
