/*
porto UDP: 3200
porto TCP: 3100
*/

#include "header.h"
int config;

void server_config();
void server_to_clients();
void process_config(int fd_config);
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("server <client port> <config port> <register file>");
        exit(-1);
    }
    pid_t config_id, clients_id;

    if ((config_id = fork()) == 0)
    {
        server_config(argv[2]);
        exit(0);
    }
    if ((clients_id = fork()) == 0)
    {
        server_to_clients(argv[1]);
        exit(0);
    }
}
//para tratar da administracao do servidor
void server_config(char *port_config)
{
    /*************************TCP****************************/
    struct sockaddr_in addr_server_config, config_addr;
    int config_addr_size;
    char *endServer;
    int config_addr_size = sizeof(client_addr);
    strcpy(endServer, IP_SERVER);
    int fd_config;
    if ((hostPtr = gethostbyname(endServer)) == 0)
        error("Invalid address");

    struct hostent *hostPtr;
    bzero((void *)&addr, sizeof(addr));
    addr_server_config.sin_family = AF_INET;
    addr_server_config.s_adr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr_server_config.sin_port = htons((short)atoi(port_config));
    if ((fd_config = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("socket");
    if (bind(fd_config, (struct sockaddr *)&addr_server_config, sizeof(addr_server_config)) < 0)
        error("bind");
    if (listen(fd_config, 5) < 0)
        error("listen");

    while (1)
    {
        //clean finished child processes, avoiding zombies
        //must use WNOHANG or would block whenever a child process was working
        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;
        //wait for new connection
        config = accept(fd, (struct sockaddr *)&config_addr, (socklen_t *)&config_addr_size);
        if (client > 0)
        {
            if (fork() == 0)
            {
                process_config(config, (struct sockaddr_in)config_addr);
                close(config);
                exit(0);
            }
        }
    }
}
void process_config(int fd_config, struct sockaddr_in config_addr)
{
    int nread = 0;
    char command[MESSAGE_LEN];

    do
    {
        nread = read(fd_config, command, MESSAGE_LEN - 1);
        command[nread] = '\0';

        if (!strcmp(command, "QUIT"))
            go = FALSE;
        else if (!strcmp(command, "LIST")))
            {
            }
    } while (go = TRUE && nread > 0);
}
//para tratar dos clientes
void server_to_clients(char *port_clients)
{
    /*************************UDP****************************/
    printf("Server to clients [%d]\n", getpid());
}
