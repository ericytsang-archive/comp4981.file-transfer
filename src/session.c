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
 * @function   static void initialize(int clntPid, int priority, char* filePath)
 * @function   static void terminate_program(bool clientPresent)
 * @function   static void read_loop(void)
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
static void fatal(char* str);
static void initialize(int clntPid, int priority, char* filePath);
static void terminate_program(bool clientPresent);
static void read_loop(int);

/* global variables for inter process communication */
static pid_t clientPid = 0;
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
    /* obtain system resources for the process */
    initialize(clntPid, priority, filePath);

    /* do the read loop */
    read_loop(priority);

    /* terminate program... */
    terminate_program(true);

    return 0;
}

/**
 * obtains the resources needed by the session for it to function.
 *
 * @function   initialize
 *
 * @date       2015-02-12
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  static void initialize(int clntPid, int priority, char* filePath)
 *
 * @param      clntPid process id of the session's client process
 */
static void initialize(int clntPid, int priority, char* filePath)
{
    Message pidMsg;         /* used to send client the PID of this process */
    char fatalstring[MAX_STR_LEN];  /* buffer used to print fatal messages */

    pidMsg.dataType  = MSG_DATA_PID;

    /* initialize global client PID */
    clientPid = clntPid;

    /* set signal handler */
    signal(SIGUSR1, sigusr1_handler);

    /* get the message queue. */
    get_message_queue(&msgQId);

    /* verify priority input */
    if(priority < MIN_PROC_PRIO || priority > MAX_PROC_PRIO)
    {
        sprintf(fatalstring, "invalid priority; %d <= priority <= %d\n",
            MIN_PROC_PRIO, MAX_PROC_PRIO);
        fatal(fatalstring);
    }

    /* open the file */
    fd = open(filePath, 0);
    if(fd == -1)
    {
        sprintf(fatalstring, "failed to open file: %d\n", errno);
        fatal(fatalstring);
    }

    /* send the client the session's PID */
    pidMsg.data.pidMsg.pid = getpid();
    msg_send(msgQId, &pidMsg, clientPid);
}

/**
 * performs the process's main read loop. it reads from the open file, and send
 *   it to the client through the message queue.
 *
 * @function   read_loop
 *
 * @date       2015-02-12
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note       none
 *
 * @signature  static void read_loop(void)
 *
 * @param      priority of the client
 */
static void read_loop(int priority)
{
    size_t nRead;       /* bytes read from file per read */
    Message dataMsg;    /* used to send file data to client */

    /* initialize message types */
    dataMsg.dataType = MSG_DATA_DATA;

    /* read from the file & send to client in a loop */
    do
    {
        /* read contents from the file & prepare message to send to client. */
        nRead = read(fd, dataMsg.data.dataMsg.data,
            MAX_MSG_DATAMSGDATA_LEN/priority);
        dataMsg.data.dataMsg.len = nRead;

        /* send the message to the client, and exit on error. */
        msg_send(msgQId, &dataMsg, clientPid);
    }
    while(nRead > 0);
}

/**
 * cleans up, and terminates the process.
 *
 * @function   terminate_program
 *
 * @date       2015-02-12
 *
 * @revision   none
 *
 * @designer   EricTsang
 *
 * @programmer EricTsang
 *
 * @note
 *
 * the termination process is different depending if the client is preset, still
 *   listening to the message queue or not.
 *
 * if the client is present, the process will enqueue a stop client message into
 *   the message queue.
 *
 * if the client process is no longer present, then the session will clear all
 *   messages of its type, release its resources and terminate.
 *
 * @signature  static void terminate_program(bool clientPresent)
 *
 * @param      clientPresent true if the client is still active, ready to
 *   dequeue messages from the message queue; false otherwise.
 */
static void terminate_program(bool clientPresent)
{
    /**
     * if the client is present, send stop message; clear all messages of the
     *   client type otherwise.
     */
    if(clientPresent)
    {
        /**
         * declare, initialize & send a stop message.
         */
        Message stopMsg;
        stopMsg.dataType = MSG_DATA_STOPCLNT;
        msg_send(msgQId, &stopMsg, clientPid);
    }
    else
    {
        /**
         * clear all messages for the client, so message queue isn't littered
         *   with stuff.
         */
        msg_clear_type(msgQId, clientPid);
    }

    /* release resources, and exit the program */
    close(fd);
    exit(0);
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
    if(sigNum == SIGUSR1)
    {
        terminate_program(false);
    }
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
    prntMsg.dataType = MSG_DATA_PRINT;

    /* send a print message as well as an stop message to the client */
    sprintf(prntMsg.data.printMsg.str, "fatal: %s", str);
    msg_send(msgQId, &prntMsg, clientPid);

    /* terminate program... */
    terminate_program(true);
}
