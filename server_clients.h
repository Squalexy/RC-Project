#ifndef SERVER_CLIENTS_H
#define SERVER_CLIENTS_H

#include "global.h"
#include "file.h"

void server_to_clients(char *);
typedef struct node node_t;
struct node {
    user_t user;
    node_t * next;
};

#endif 