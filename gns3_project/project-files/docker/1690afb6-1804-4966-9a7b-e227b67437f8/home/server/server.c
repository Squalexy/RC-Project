/*
porto UDP: 3200
porto TCP: 3100
*/

#include "global.h"
#include "server.h"

int config;

void server_config();
void server_to_clients();
void process_config(int fd_config, struct sockaddr_in config_addr);
void init();
void clean();
int main(int argc, char *argv[])
{
    signal(SIGINT, SIG_IGN);
    init();
    if (argc != 4)
    {
        printf("server <client port> <config port> <register file>");
        exit(-1);
    }
    strcpy(clients_file, argv[3]);
    // *************** MENU *************** //
    printf("----------------- SERVER -----------------\n");
    printf("IP ADDRESS: %s\n", IP_SERVER_PRIVATE);
    printf("UDP PORT: %s\n", argv[1]);
    printf("TCP PORT: %s\n", argv[2]);
    pid_t config_pid, clients_pid;
    //for configurations

    if ((config_pid = fork()) == 0)
    {
        server_config(argv[2]);
        exit(0);
    }

    //hendles clients requests
    if ((clients_pid = fork()) == 0)
    {
        server_to_clients(argv[1]);
        exit(0);
    }

    waitpid(config_pid, NULL, 0);
    waitpid(clients_pid, NULL, 0);
    clean();
    printf("SERVER IS CLOSING\n");
}
void init()
{
    init_mutex_registers();
}
void clean() {
    close_mutex_registers();
}