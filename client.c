#include "header.h"

// cliente {endereço do servidor} {porto}
// ligação UDP

#define PORT 3200

void erro(char *msg)
{
    printf("Erro: %s\n", msg);
    exit(-1);
}

int main(int argc, char *argv[])
{

    int fd, recv_len;
    char buffer[MESSAGE_LEN];
    struct hostent *hostPtr;
    char endServer[100];
    if (argc != 3)
        erro("cliente <endereço do servidor> <porto>");

    strcpy(endServer, argv[1]);
    if (argv[1] != "193.136.212.243")
    {
        erro("Endereço do servidor errado (interface externa de R3");
    }
    if ((hostPtr = gethostbyname(endServer)) == 0)
        erro("Não consegui obter endereço");

    struct sockaddr_in addr_server;
    socklen_t slen = sizeof(addr_server);

    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        erro("socket");

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons((short)atoi(argv[2]));
    addr_server.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;

    char username[16];
    char password[16];

    while (1)
    {
        printf("Username: ");
        if (!fgets(username, sizeof(username), stdin))
        {
            printf("Wrong format for username. Try again.\n");
        }
    }

    while (1)
    {
        printf("Password: ");
        if (!fgets(password, sizeof(password), stdin))
        {
            printf("Wrong format for password. Try again.\n");
        }
    }

    char id_info[strlen(username) + strlen(password) + 1];
    snprintf(id_info, strlen(username) + strlen(password) + 1, username, "%s;%s", username, password);

    sendto(fd, (const char *)id_info, strlen(id_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

    /*Retirado de: https://stackoverflow.com/questions/15941005/making-recvfrom-function-non-blocking*/

    struct timeval read_timeout;
    read_timeout.tv_sec = 5;
    read_timeout.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

    if ((recv_len = recvfrom(fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
        erro("Erro no recvfrom");

    buffer[recv_len] = '\0';
    printf("%s\n", buffer);

    close(fd);
    return 0;
}