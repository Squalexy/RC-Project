#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

#define BUFFFER_LEN 512
#define PORT 9876

void erro(char *msg)
{
    printf("Erro: %s\n", msg);
    exit(-1);
}

int main(int argc, char *argv[])
{

    int fd, recv_len;
    char buffer[BUFFFER_LEN];
    struct hostent *hostPtr;
    char endServer[100];
    if (argc != 4)
        erro("cliente <host> <port> <string>");

    strcpy(endServer, argv[1]);
    if ((hostPtr = gethostbyname(endServer)) == 0)
        erro("Nao consegui obter endereço");

    struct sockaddr_in addr_server;
    socklen_t slen = sizeof(addr_server);

    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        erro("socket");

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons((short)atoi(argv[2]));
    addr_server.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;

    sendto(fd, (const char *)argv[3], strlen(argv[3]), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

    /*Retirado de: https://stackoverflow.com/questions/15941005/making-recvfrom-function-non-blocking*/

    struct timeval read_timeout;
    read_timeout.tv_sec = 5;
    read_timeout.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

    if ((recv_len = recvfrom(fd, buffer, BUFFFER_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
        erro("Erro no recvfrom");

    buffer[recv_len] = '\0';
    printf("%s\n", buffer);

    close(fd);
    return 0;
}