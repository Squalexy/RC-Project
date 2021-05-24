#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

#define MESSAGE_LEN 512
#define GROUP_NAME_LEN 100
#define IP_SERVER "193.136.212.243"
#define ERROR "ERROR"
#define CLIENT_PORT 3100
#define TRUE 1
#define FALSE 0
#define SIZE 20
#define DELIM ";"
#define SEND_MESSAGE "SEND_MESSAGE"
#define ACCESS_GROUP "ACCESS_GROUP"
#define CREATE_GROUP "CREATE_GROUP"
#define REQUEST_P2P "REQUEST_P2P"
#define LOGIN "LOGIN"
#define DISCONNECT "DISCONNECT"


void error(char *msg);

#endif
