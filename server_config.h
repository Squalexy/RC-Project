#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "global.h"
#include "file.h"

#define GROUPS_FILE "groups.bin"
void server_config(char *);

typedef struct group {
    char group_name [20];
    char multicast_address [38] ;
    char port [10];
    user_t owner;
}group_t;

#endif