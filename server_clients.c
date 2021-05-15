
#include "server_clients.h"
//para tratar dos clientes
void server_to_clients(char *port_clients)
{
    /*************************UDP****************************/
    printf("Server to clients [%d]\n", getpid());
    int fd;
    struct sockaddr_in addr_server, addr_client;
    socklen_t client_len = sizeof(addr_client);

    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        perror("Error in socket\n");

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port_clients);
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *)&addr_server, sizeof(addr_server)) == -1)
        perror("Error in bind");
    char buffer[MESSAGE_LEN];
    int rclen;
    while (TRUE)
    {
        if ((rclen = recvfrom(fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_client, (socklen_t *)&client_len)) == -1)
            perror("Error in recvfrom");
        buffer[rclen] = '\0';
        // TRATAR DA MENSAGEM;
    }
}
