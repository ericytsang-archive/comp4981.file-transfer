#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>


#define CHILD_PID 0
#define MSG_FLGS 0644 | IPC_CREAT

/* structure definitions */
struct Message
{
    long msgType;
    char* text;
};

typedef struct Message Message;

/* function prototypes */
int child(int msq_id);
int parent(int msq_id);

/**
 * sets up the processes, and the IPC structure.
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
    key_t mkey;
    int msq_id;

    /* verify input */
    if (argc != 2)
    {
        printf("Usage: showmsg keyval\n");
        return 1;
    }

    /* Get message queue identifier */
    mkey = (key_t) atoi(argv[1]);
    if ((msq_id = msgget(mkey, MSG_FLGS)) < 0)
    {
        perror("msgget failed!");
        return 2;
    }

    if(fork() == CHILD_PID)
    {
        return child(msq_id);
    }
    else
    {
        return parent(msq_id);
    }
}

/**
 * run on the child process. this process reads from the message queue.
 *
 * @function   child
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
 * @signature  int child(int msq_id)
 *
 * @param      msq_id id of the message queue.
 *
 * @return     return code, indicating the nature of program termination.
 */
int child(int msq_id)
{
    Message msg;
    size_t bytesRead = 0;
    bytesRead = msgrcv(msq_id, &msg, sizeof(Message), 1, 0);
    printf("%d : %s : %d\n", (int) bytesRead, msg.text, errno);
    bytesRead = msgrcv(msq_id, &msg, sizeof(Message), 1, 0);
    printf("%d : %s : %d\n", (int) bytesRead, msg.text, errno);
    bytesRead = msgrcv(msq_id, &msg, sizeof(Message), 1, 0);
    printf("%d : %s : %d\n", (int) bytesRead, msg.text, errno);
    bytesRead = msgrcv(msq_id, &msg, sizeof(Message), 1, 0);
    printf("%d : %s : %d\n", (int) bytesRead, msg.text, errno);
    return 0;
}

/**
 * run on the parent process; this function writes to the message queue, and
 *   then removes it.
 *
 * @function   parent
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
 * @signature  int parent(int msq_id)
 *
 * @param      msq_id id of the message queue
 *
 * @return     return code, indicating the nature of process termination.
 */
int parent(int msq_id)
{
    /* sending message */
    Message msg;
    msg.msgType = 1;
    msg.text = "hey there";
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);

    /* Remove the message queue */
    if (msgctl (msq_id, IPC_RMID, 0) < 0)
    {
        perror ("msgctl (remove queue) failed!");
        exit (3);
    }

    return 0;
}
