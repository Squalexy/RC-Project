#ifndef SERVER_CLIENTS_H
#define SERVER_CLIENTS_H

#include <signal.h>
#include "global.h"
#include "file.h"
#define GROUPS_FILE "groups.bin"
#define MAX_GROUPS 16
void server_to_clients(char *port_clients);
typedef struct node node_t;
struct node {
    user_t user;
    node_t * next;
};

typedef struct group {
    char group_name [SIZE];
    char multicast_address[INET_ADDRSTRLEN];
    short port;
    user_t owner;
}group_t;



// TODO: multicast addressess 
#endif 