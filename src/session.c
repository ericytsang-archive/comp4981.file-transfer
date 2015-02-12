/**
 * interfaces with the client process.
 *
 * @sourceFile session.c
 *
 * @program    server.out
 *
 * @function   static int serve_client(pid_t clntPid, int priority, char*
 *   filePath)
 * @function   static int set_process_priority(int priority)
 * @function   static void sigusr1_handler(int sigNum)
 * @function   static void fatal(char* str)
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
#include "session.h"

#define MAX_STR_LEN 80

/* function prototypes */
static void sigusr1_handler(int sigNum);
static int set_process_priority(int priority);
static void fatal(char* str);

/* global variables for inter process communication */
static pid_t clientPid;
static int msgQId;

/* file descriptor to read to the client process */
static int fd;

/**
 * takes care of the client process.
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
 * @note
 *
 * reads the contents at the location specified by filePath, and sets the
 *   process's priority to the passed one then writes the contents of the file
 *   to the message queue for the client process to read.
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
    Message dataMsg;    /* used to send file data to client */
    Message pidMsg;     /* used to send the client the PID of the session */
    char fatalstring[MAX_STR_LEN];

    /* initialize message types */
    stopMsg.dataType = MSG_DATA_STOPCLNT;
    dataMsg.dataType = MSG_DATA_DATA;
    pidMsg.dataType  = MSG_DATA_PID;

    /* set signal handler */
    signal(SIGUSR1, sigusr1_handler);

    /* initialize static variables */
    clientPid = clntPid;

    /* get the message queue. */
    get_message_queue(&msgQId);

    /* verify priority */
    if(priority < MIN_PROC_PRIO || priority > MAX_PROC_PRIO)
    {
        sprintf(fatalstring, "invalid priority; %d <= priority <= %d\n",
            MIN_PROC_PRIO, MAX_PROC_PRIO);
        fatal(fatalstring);
    }

    /* set priority */
    if(set_process_priority(priority) != 0)
    {
        sprintf(fatalstring, "failed to set priority: %d\n", errno);
        fatal(fatalstring);
    }

    /* send the client the session's PID */
    pidMsg.data.pidMsg.pid = getpid();
    msg_send(msgQId, &pidMsg, clientPid);

    /* open the file */
    fd = open(filePath, 0);
    if(fd == -1)
    {
        sprintf(fatalstring, "failed to open file: %d\n", errno);
        fatal(fatalstring);
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

/**
 * sets the priority of this process. if it fails, the program exits.
 *
 * @function   set_process_priority
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
 * @signature  static int set_process_priority(int priority)
 *
 * @param      priority new priority of the process. this is a number from 0, to
 *   20, where 0 means the process is more important.
 *
 * @return     -1 if the priority fails to be set; 0 otherwise.
 */
static int set_process_priority(int priority)
{
    return setpriority(PRIO_PROCESS, getpid(), priority);
}

/**
 * handler for the SIGUSR1 signal. this signal is used to inform this process
 *   that its client process has ended.
 *
 * @function   sigusr1_handler
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
 * @signature  static void sigusr1_handler(int sigNum)
 *
 * @param      sigNum nimber that indicates which signal this is. in this case,
 *   this number will always be SIGUSR1
 */
static void sigusr1_handler(int sigNum)
{
    msg_clear_type(msgQId, clientPid);
    close(fd);
    exit(sigNum);
}

/**
 * prints the passed message, and ends the program while cleaning up.
 *
 * @function   fatal
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
 * @signature  static void fatal(char* str)
 *
 * @param      str pointer to the first character of a string to send to the
 *   client to print on exit.
 */
static void fatal(char* str)
{
    /* declare and initialize a print & stop message structures */
    Message prntMsg;
    Message stopMsg;
    prntMsg.dataType = MSG_DATA_PRINT;
    stopMsg.dataType = MSG_DATA_STOPCLNT;

    /* send a print message as well as an stop message to the client */
    sprintf(prntMsg.data.printMsg.str, "fatal: %s", str);
    msg_send(msgQId, &prntMsg, clientPid);
    msg_send(msgQId, &stopMsg, clientPid);

    /* release resources */
    close(fd);

    exit(0);
}
