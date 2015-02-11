#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include "messagequeuehelper.h"

/* function prototypes */
static void parse_message_q_msg(void*);
static void parse_application_msg(void*);
static void message_queue_read_loop(int);

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
    int msgQId;

    /* create the message queue. */
    get_message_queue(&msgQId);

    /* set up signal handler to remove ipc. */
    signal(SIGINT, terminate_program);

    /* execute main loop of the server. */
    exitCode = message_queue_read_loop(msgQId);

    /* remove message queue. */
    remove_message_queue(msgQId);

    /* end program... */
    return exitCode;
}

/**
 * blocking function. this is the loop that reads from the message queue, and passes them on to handler functions.
 *
 * @function   message_queue_read_loop
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
 * @signature  static void message_queue_read_loop(int msgQId)
 *
 * @param      msgQId [description]
 *
 * @return     0 upon normal loop exit (EOF); 1 otherwise (error).
 */
static int message_queue_read_loop(int msgQId)
{
    sleep(5);
    return 0;
}

static int terminate_program(int sigNum)
{
    remove_message_queue(msgQId);
    exit(0);
}
