#include "global.h"

// cliente {endereço do servidor} {porto}
// ligação UDP

// diferente do TCP:
// read --> recvfrom
// write --> sendto

#define PORT 3200

/* ---------------------------------------------------------------------------------
---------------------------------------- MAIN --------------------------------------
------------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{

    // ------------------ DECLARAÇÕES VARIÁVEIS ------------------ //

    int fd, recv_len;
    char buffer[MESSAGE_LEN];
    struct hostent *hostPtr;
    struct sockaddr_in addr_server;
    socklen_t slen = sizeof(addr_server);
    char endServer[100];

    // ------------------ EXECUTAR PROGRAMA CLIENTE ------------------ //

    execute_client(argc, endServer, hostPtr, addr_server, fd);

    // ------------------ AUTENTIFICAÇÃO ------------------ //

    char username[16], password[16];
    authentication(fd, addr_server, username, password);

    recvfrom_nonblocking(fd);

    if ((recv_len = recvfrom(fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
        error("Erro no recvfrom");

    communication(fd);
    buffer[recv_len] = '\0';
    printf("%s\n", buffer);

    close(fd);
    return 0;
}

/* ---------------------------------------------------------------------------------
-------------------------------------- FUNÇÕES -------------------------------------
------------------------------------------------------------------------------------*/

void execute_client(int argc, char *endServer, struct hostent *hostPtr, struct sockaddr_in addr_server, int fd)
{
    if (argc != 3)
        error("cliente <endereço do servidor> <porto>");

    strcpy(endServer, argv[1]);
    if (argv[1] != "193.136.212.243")
    {
        error("endereço tem que ser 193.136.212.243 (interface externa de R3");
    }
    if ((hostPtr = gethostbyname(endServer)) == 0)
        error("não consegui obter endereço");

    socklen_t slen = sizeof(addr_server);

    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        error("socket");

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons((short)atoi(argv[2]));
    addr_server.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
}

void authentication(int fd, struct sockaddr_in addr_server, char *username, char *password)
{
    while (1)
    {
        printf("Username: ");
        if (!fgets(username, sizeof(username), stdin))
        {
            printf("Formato errado do username. Tente novamente...\n");
        }
    }

    while (1)
    {
        printf("Password: ");
        if (!fgets(password, sizeof(password), stdin))
        {
            printf("Formato errado da password. Tente novamente...\n");
        }
    }

    char id_info[strlen(username) + strlen(password) + 1];
    snprintf(id_info, strlen(username) + strlen(password) + 1, username, "%s;%s", username, password);
    sendto(fd, (const char *)id_info, strlen(id_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));
}

void communication(int server_fd)
{
    printf("CONNECTED TO THE SERVER\n\n");
    int go = TRUE;
    char command[MESSAGE_LEN];
    int nread;

    do
    {

        input_menu();
        get_one_line(stdin, command, MESSAGE_LEN);
        send_to_server(server_fd, command);

        // CLIENT-SERVER
        if (!strcmp(command, "1"))
        {
        }

        // P2P
        else if (!strcmp(command, "2"))
        {
        }

        // GROUP
        else if (!strcmp(command, "3"))
        {
        }

        // EXIT
        else if (!strcmp(command, "4"))
        {
            go = FALSE;
        }

    } while (go && nread > 0);
}

void recvfrom_nonblocking(int fd)
{
    // Retirado de: https://stackoverflow.com/questions/15941005/making-recvfrom-function-non-blocking
    struct timeval read_timeout;
    read_timeout.tv_sec = 5;
    read_timeout.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
}