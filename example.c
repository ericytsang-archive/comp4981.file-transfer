/* showmsg.c: Show message queue details */
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define CHILD_PID 0

/* Structure definitions */
typedef struct
{
    long msgType;
    char* text;
}
Message;


/* Function prototypes */
void mqstat_print (key_t mkey, int mqid, struct msqid_ds *mstat);
int child(int msq_id);

int main (int argc , char *argv[])
{
    key_t mkey;
    int msq_id;
    struct msqid_ds msq_status;

    if (argc != 2)
    {
        fprintf (stderr, "Usage: showmsg keyval\n");
        exit(1);
    }

    /* Get message queue identifier */
    mkey = (key_t) atoi(argv[1]);
    if ((msq_id = msgget (mkey, IPC_CREAT)) < 0)
    {
        perror ("msgget failed!");
        exit(2);
    }

    if(fork() == CHILD_PID)
    {
        return child(msq_id);
    }

    Message msg;
    msg.msgType = 1;
    msg.text = "hey there";
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);

    /* get status info */
    /*if (msgctl (msq_id, IPC_STAT, &msq_status) < 0)
    {
        perror ("msgctl (get status)failed!");
        exit(3);
    }*/

    /* print out status information */
    /*mqstat_print (mkey, msq_id, &msq_status);*/

    /* Remove he message queue */
    if (msgctl (msq_id, IPC_RMID, 0) < 0)
    {
        perror ("msgctl (remove queue) failed!");
        exit (3);
    }

    exit(0);
}

int child(int msq_id)
{
    Message msg;
    size_t bytesRead = 0;
    bytesRead = msgrcv(msq_id, &msg, sizeof(Message), 1, 0);
    printf("%d : %s\n", bytesRead, msg.text);
    bytesRead = msgrcv(msq_id, &msg, sizeof(Message), 1, 0);
    printf("%d : %s\n", bytesRead, msg.text);
    bytesRead = msgrcv(msq_id, &msg, sizeof(Message), 1, 0);
    printf("%d : %s\n", bytesRead, msg.text);
    bytesRead = msgrcv(msq_id, &msg, sizeof(Message), 1, 0);
    printf("%d : %s\n", bytesRead, msg.text);
    return 0;
}

/* status info print function */
void mqstat_print (key_t mkey, int mqid, struct msqid_ds *mstat)
{
    /* call the library function ctime */
    // char *ctime();

    printf ("\nKey %d, msg_qid %d\n\n", mkey, mqid);
    printf ("%d messages on queue\n\n", (int)mstat->msg_qnum);
    printf ("Last send by proc %d at %s\n",
        mstat->msg_lspid, ctime(&(mstat->msg_stime)));
    printf ("Last recv by proc %d at %s\n",
        mstat->msg_lrpid, ctime(&(mstat->msg_rtime)));
}
