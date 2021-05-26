#ifndef SERVER_H
#define SERVER_H
#include <signal.h>
#include "global.h"
#include "file.h"
#include <stdlib.h>
/*************** PROJETO DE REDES DE COMUNICACAO ***************/
// Joana Simoes, No 2019217013
// Alexy de Almeida No 2019192123

#define IP_SERVER_PRIVATE "10.90.0.1"
#define MAX_CLIENTS 4

//* ------------------ SERVER_CONFIG ------------------

void server_config(char *);

//* ------------------ SERVER_CLIENTS ------------------

typedef struct group
{
    char group_name[20];
    char multicast_address[38];
    char port[10];
    user_t owner;
} group_t;
#define GROUPS_FILE "groups.bin"
#define MAX_GROUPS 16

void server_to_clients(char *port_clients);

#endif