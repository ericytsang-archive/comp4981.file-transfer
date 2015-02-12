/**
 * header file for session.c, exposing its interface.
 *
 * @sourceFile session.h
 *
 * @program    server.out
 *
 * @function   int serve_client(pid_t clientPid, int priority, char* filePath);
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
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include "messagequeuehelper.h"

#define MIN_PROC_PRIO 1
#define MAX_PROC_PRIO 20

int serve_client(pid_t clientPid, int priority, char* filePath);
