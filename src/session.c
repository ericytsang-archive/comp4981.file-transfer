#include "session.h"

static void sigusr1_handler(int sigNum);
static void set_process_priority(int priority);

static pid_t clientPid;
static int msgQId;
static int fd;

/**
 * run on a separate process forked from the client to take care of a client's
 *   request.
 *
 * @function   serve_client
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
 * @signature  static int serve_client(pid_t clntPid, int priority, char*
 *   filePath)
 *
 * @param      clntPid process id of the child process
 * @param      priority priority of this client object
 * @param      filePath path to file to send to client through IPC
 *
 * @return     returns 0, normal exit return code.
 */
int serve_client(pid_t clntPid, int priority, char* filePath)
{
    int fd;             /* file to read to client */
    size_t nRead;       /* bytes read from file per read */
    Message stopMsg;    /* ends the client process */
    Message prntMsg;    /* makes client print message contents to screen */
    Message dataMsg;    /* used to send file data to client */
    Message pidMsg;     /* used to send the client the PID of the session */

    /* initialize message types */
    stopMsg.dataType = MSG_DATA_STOPCLNT;
    prntMsg.dataType = MSG_DATA_PRINT;
    dataMsg.dataType = MSG_DATA_DATA;
    pidMsg.dataType  = MSG_DATA_PID;

    /* initialize static variables */
    clientPid = clntPid;

    /* get the message queue. */
    get_message_queue(&msgQId);

    /* set signal handler */
    signal(SIGUSR1, sigusr1_handler);

    /* verify priority */
    if(priority < MIN_PROC_PRIO || priority > MAX_PROC_PRIO)
    {
        sprintf(prntMsg.data.printMsg.str,
            "invalid priority; %d <= priority <= %d\n",
            MIN_PROC_PRIO, MAX_PROC_PRIO);
        msg_send(msgQId, &prntMsg, clientPid);
        msg_send(msgQId, &stopMsg, clientPid);
        return 0;
    }

    /* set priority */
    set_process_priority(priority);

    /* send the client the session's PID */
    pidMsg.data.pidMsg.pid = getpid();
    msg_send(msgQId, &pidMsg, clientPid);

    /* open the file */
    fd = open(filePath, 0);
    if(fd == -1)
    {
        sprintf(prntMsg.data.printMsg.str,
            "failed to open file: %d\n", errno);
        msg_send(msgQId, &prntMsg, clientPid);
        msg_send(msgQId, &stopMsg, clientPid);
        return 0;
    }

    /* read from the file & send to client in a loop */
    do
    {
        /* read contents from the file & prepare message to send to client. */
        nRead = read(fd, dataMsg.data.dataMsg.data, MAX_MSG_DATAMSGDATA_LEN);
        dataMsg.data.dataMsg.len = nRead;

        /* send the message to the client, and exit on error. */
        msg_send(msgQId, &dataMsg, clientPid);
    }
    while(nRead > 0);

    /* clean up; release resources */
    close(fd);

    /* stop the client. */
    msg_send(msgQId, &stopMsg, clientPid);

    return 0;
}

static void set_process_priority(int priority)
{
    if(setpriority(PRIO_PROCESS, getpid(), priority) != 0)
    {
        Message stopMsg;    /* ends the client process */
        Message prntMsg;    /* makes client print message contents to screen */

        stopMsg.dataType = MSG_DATA_STOPCLNT;
        prntMsg.dataType = MSG_DATA_PRINT;

        sprintf(prntMsg.data.printMsg.str,
            "failed to set priority: %d\n", errno);

        msg_send(msgQId, &prntMsg, clientPid);
        msg_send(msgQId, &stopMsg, clientPid);

        exit(0);
    }
}

static void sigusr1_handler(int sigNum)
{
    msg_clear_type(msgQId, clientPid);
    close(fd);
    exit(sigNum);
}
