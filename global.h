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

#include "functions.c"

#define MESSAGE_LEN 512
#define CLIENTS_FILE "clients.txt"
#define AUX_FILE "aux.txt"
#define IP_SERVER "193.136.212.243"
#define TRUE 1
#define FALSE 0



#endif