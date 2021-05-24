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
#define IP_SERVER "193.136.212.243" // TODO: mudar este endereço
#define TRUE 1
#define FALSE 0
#define SIZE 20
#define SEND_MESSAGE "SEND_MESSAGE"
#define ACCESS_GROUP "ACCESS_GROUP"
#define CREATE_GROUP "CREATE_GROUP"
#define REQUEST_P2P "REQUEST_P2P"
#define LOGIN "LOGIN"
#define MAX_CLIENTS 4

typedef struct {
    char user_id [20];
    char ip [INET_ADDRSTRLEN];
    char password [20];
    char client_server [10];
    char p2p [10];
    char group [10];
    char port [10];
} user_t;
#endif
