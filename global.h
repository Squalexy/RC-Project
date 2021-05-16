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
#define CLIENTS_FILE "clients.bin"
#define AUX_FILE "aux.bin"
#define IP_SERVER "127.0.0.1" //"193.136.212.243"
#define TRUE 1
#define FALSE 0


typedef struct {
    char user_id[20];
    char ip [38];
    char password[20];
    char client_server[10];
    char p2p[10];
    char group[10];
}user_t;
#endif
