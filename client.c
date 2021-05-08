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

    execute_client(argc, argv, endServer, hostPtr, addr_server, fd);

    // ------------------ AUTENTIFICAÇÃO ------------------ //

    char username[16], password[16];
    authentication(fd, addr_server, username, password);

    recvfrom_nonblocking(fd);

    if ((recv_len = recvfrom(fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
        error("Erro no recvfrom");

    printf("Authentication successful!\n");

    communication(fd, addr_server, username);

    close(fd);
    return 0;
}

/* ---------------------------------------------------------------------------------
-------------------------------------- FUNÇÕES -------------------------------------
------------------------------------------------------------------------------------*/

void execute_client(int argc, char *argv, char *endServer, struct hostent *hostPtr, struct sockaddr_in addr_server, int fd)
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

void communication(int server_fd, struct sockaddr_in addr_server, char *username)
{
    printf("CONNECTED TO THE SERVER\n\n");
    int go = TRUE;
    int recv_len, nread;
    socklen_t slen = sizeof(addr_server);
    char command[MESSAGE_LEN], command_group[MESSAGE_LEN], buffer[MESSAGE_LEN];
    char user[16];

    do
    {

        input_menu();
        get_one_line(stdin, command, MESSAGE_LEN);

        // CLIENT-SERVER
        if (!strcmp(command, "1"))
        {
            //? PEDIDO DE COMUNICAÇÃO

            char c_s_info[MESSAGE_LEN] = "";
            input_user();
            get_one_line(stdin, user, 16);

            snprintf(c_s_info, strlen(username) + strlen(user) + 1, username, "User %s is asking for CLIENT/SERVER communication with user %s.\n", username, user);
            sendto(server_fd, (const char *)c_s_info, strlen(c_s_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));
        }

        // P2P
        else if (!strcmp(command, "2"))
        {
            //? PEDIDO DE COMUNICAÇÃO

            char p2p_info[MESSAGE_LEN] = "";

            snprintf(p2p_info, strlen(username), username, "User %d is asking for P2P communication.\n--> Send UDP address and port to client.\n");
            sendto(server_fd, (const char *)p2p_info, strlen(p2p_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

            //? RECEBER ENDEREÇO E PORTO UDP

            recvfrom_nonblocking(server_fd);

            if ((recv_len = recvfrom(server_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
            {
                error("Erro no recvfrom");
            }

            buffer[recv_len] = '\0';
            printf("Received UDP address and port: %s\n", buffer);

            //TODO: iniciar comunicação P2P
        }

        // GROUP
        else if (!strcmp(command, "3"))
        {

            group_comm_create();
            get_one_line(stdin, command_group, MESSAGE_LEN);

            if (!strcmp(command_group, "1"))
            {
                //? PEDIDO DE COMUNICAÇÃO

                char group_info[MESSAGE_LEN] = "";

                snprintf(group_info, strlen(username), username, "User %d is asking to CREATE GROUP.\n--> Send group multicast address.\n");
                sendto(server_fd, (const char *)group_info, strlen(group_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

                recvfrom_nonblocking(server_fd);

                //? RECEBER ENDEREÇO MULTICAST

                if ((recv_len = recvfrom(server_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
                {
                    error("Erro no recvfrom");
                }

                buffer[recv_len] = '\0';
                printf("Received group multicast address: %s\n", buffer);

                //TODO: iniciar comunicação grupo
            }

            else if (!strcmp(command_group, "2"))
            {
                //? PEDIDO DE COMUNICAÇÃO

                char group_info[MESSAGE_LEN] = "";

                //! Aqui não sei o que é preciso pedir ao servidor: nome do grupo ou endereço multicast do grupo?

                snprintf(group_info, strlen(username), username, "User %d is asking to CREATE GROUP.\n--> Send group multicast address.\n");
                sendto(server_fd, (const char *)group_info, strlen(group_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

                //? RECEBER O ENDEREÇO MULTICAST A USAR

                if ((recv_len = recvfrom(server_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
                {
                    error("Erro no recvfrom");
                }

                buffer[recv_len] = '\0';
                printf("Received group multicast address: %s\n", buffer);

                //TODO: iniciar comunicação grupo
            }
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

int received_from_server(int server_fd)
{
    char buffer[MESSAGE_LEN - 1];
    int nread = read(server_fd, buffer, MESSAGE_LEN);
    buffer[nread] = '\0';
    printf("%s\n", buffer);
    return nread;
}