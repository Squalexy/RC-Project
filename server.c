/*
porto UDP: 3200
porto TCP: 3100
*/

#include "global.h"
#include "server_config.h"

int config;

void server_config();
void server_to_clients();
void process_config(int fd_config, struct sockaddr_in config_addr);
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("server <client port> <config port> <register file>");
        exit(-1);
    }

    // *************** MENU *************** //
    printf("----------------- SERVER -----------------")
    printf("IP ADDRESS: %s\n", IP_SERVER);
    printf("UDP PORT:%s\n", argv[1]);
    printf("TCP PORT\n", argv[2]);
    pid_t config_pid, clients_pid;
    //para administracoes do servidor
    if ((config_pid = fork()) == 0)
    {
        server_config(argv[2]);
        exit(0);
    }
    //para interagir com os clientes
    if ((clients_pid = fork()) == 0)
    {
        server_to_clients(argv[1]);
        exit(0);
    }

    waitpid(config_pid, NULL);
    waitpod(clients_pid, NULL);

    printf("SERVER IS CLOSING\n");
}
