#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "global.h"
#include "file.h"
void server_config(char *);

typedef struct
{
    char user_id[20];
    char ip[38];
    char password[20];
    char client_server[10];
    char p2p[10];
    char group[10];
} user_t;

typedef struct{
    int user_ip;
    char ip[38];
}group_t;

#endif