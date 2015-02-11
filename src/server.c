#include "server.h"

/* function prototypes */
static void sigint_handler(int);
static int msgq_read_loop(int);
static bool parse_msgq_msg(Message*);
static void handle_connect_msg(ConnectMsg*);
static int serve_client(pid_t, int, char*);

/**
 * message queue id used by the server.
 */
static int msgQId;

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
    int exitCode;

    /* create the message queue. */
    get_message_queue(&msgQId);

    /* set up signal handler to remove IPC. */
    signal(SIGINT, sigint_handler);

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
    exit(0);
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
    static int breakMsgLoop = 0;
    Message msg;

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
        printf("MSG_DATA_STOP_SVR\n");
        returnVal = false;
        break;
    case MSG_DATA_CONNECT:
        printf("MSG_DATA_CONNECT\n");
        handle_connect_msg(&msg->data.connectMsg);
        returnVal = true;
        break;
    }

    return returnVal;
}

/**
 * handles the connection request message.
 *
 * @function   [class_header] [method_header]
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
 * @signature  [some_headers_only] [class_header] [file_header]
 *
 * @param      msg [description]
 */
static void handle_connect_msg(ConnectMsg* connectMsg)
{
    /* create a new process to serve the client */
    if(fork() == 0)
    {
        int returnValue;

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

static int serve_client(pid_t clientPid, int priority, char* filePath)
{
    int fd;             /* file to read to client */
    size_t nRead;       /* bytes read from file per read */
    Message stopMsg;    /* ends the client process */
    Message prntMsg;    /* makes client print message contents to screen */
    Message dataMsg;    /* used to send file data to client */

    /* initialize message types */
    stopMsg.dataType = MSG_DATA_STOPCLNT;
    prntMsg.dataType = MSG_DATA_PRINT;
    dataMsg.dataType = MSG_DATA_DATA;

    /* verify priority */
    if(priority < 0 || priority > 10)
    {
        sprintf(prntMsg.data.printMsg.str,
            "invalid priority; 0 <= priority <= 10\n");
        msg_send(msgQId, &prntMsg, clientPid);
        msg_send(msgQId, &stopMsg, clientPid);
    }

    /* set priority */
    if(setpriority(PRIO_PROCESS, getpid(), priority) == -1)
    {
        sprintf(prntMsg.data.printMsg.str, "failed to set priority: %d\n", errno);
        msg_send(msgQId, &prntMsg, clientPid);
        msg_send(msgQId, &stopMsg, clientPid);
    }

    /* open the file */
    fd = open(filePath, 0);
    if(fd == -1)
    {
        sprintf(prntMsg.data.printMsg.str, "failed to open file: %d\n", errno);
        msg_send(msgQId, &prntMsg, clientPid);
        msg_send(msgQId, &stopMsg, clientPid);
    }

    /* read from the file & send to client in a loop */
    do
    {
        nRead = read(fd, dataMsg.data.dataMsg.data,
            MAX_MSG_DATAMSGDATA_LEN);
        dataMsg.data.dataMsg.len = nRead;
        msg_send(msgQId, &dataMsg, clientPid);
    }
    while(nRead > 0);

    /* clean up; release resources */
    close(fd);

    /* stop the client. */
    msg_send(msgQId, &stopMsg, clientPid);

    return 0;
}
