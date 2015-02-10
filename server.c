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
struct Message
{
    long msgType;
    char* text;
};

typedef struct Message Message;

/* Function prototypes */
int child(int msq_id);
int parent(int msq_id);

int main (int argc , char *argv[])
{
    key_t mkey;
    int msq_id;

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
    else
    {
        parent(msq_id);
    }

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

int parent(int msq_id)
{
    Message msg;
    msg.msgType = 1;
    msg.text = "hey there";
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);
    msgsnd(msq_id, &msg, sizeof(Message), 1);
    sleep(1);
    return 0;
}
