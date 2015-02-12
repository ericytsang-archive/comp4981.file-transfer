#include "client.h"

/* function prototypes */
static void msgq_loop(int msgQId);
static void connect(int msgQId, int priority, char* filePath);
static void sigint_handler(int sigNum);

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
 * @signature  int main (int argc , char** argv)
 *
 * @param      argc number of command line arguments, including the program
 *   name.
 * @param      argv array of c-style character arrays that are the command line
 *   arguments.
 *
 * @return     return code, indication the nature of process termination.
 */
int main (int argc , char** argv)
{

    /* verify command line arguments */
    if(argc != 3)
    {
        printf("usage: %s [priority] [filepath]\n", argv[0]);
        exit(0);
    }

    /* set signal handler */
    signal(SIGINT, sigint_handler);

    /* get the message queue. */
    get_message_queue(&msgQId);

    /* send connection message to server */
    connect(msgQId, atoi(argv[1]), argv[2]);

    /* get messages from server until stop */
    msgq_loop(msgQId);

    /* end program... */
    return 0;
}

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

static void sigint_handler(int sigNum)
{
    kill(sessionPid, SIGUSR1);
    exit(sigNum);
}
