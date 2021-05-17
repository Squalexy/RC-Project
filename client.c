#include "global.h"

// cliente {endereço do servidor} {porto}
// ligação UDP

// diferente do TCP:
// read --> recvfrom
// write --> sendto

#define PORT 3200

/*
*-----------------------------------------------------------------------------------------------------------------------------------
*--------------------------------------------------------------- MAIN -----------------------------------------------------------
*-----------------------------------------------------------------------------------------------------------------------------------
*/

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

    // ------------------ COMUNICACAÇÃO ------------------ //

    communication(fd, addr_server, username);

    close(fd);
    return 0;
}

/*
*-----------------------------------------------------------------------------------------------------------------------------------
*--------------------------------------------------------------- FUNÇÕES -----------------------------------------------------------
*-----------------------------------------------------------------------------------------------------------------------------------
*/

void execute_client(int argc, char *argv, char *endServer, struct hostent *hostPtr, struct sockaddr_in addr_server, int fd)
{
    if (argc != 3)
        error("cliente <endereço do servidor> <porto>");

    strcpy(endServer, argv[1]);
    if (argv[1] != "193.136.212.243")
    {
        error("endereço tem que ser 193.136.212.243 (interface externa de R3");
    }
    if ((hostPtr = gethostbyname(endServer)) == 0) // IP
        error("não consegui obter endereço");

    socklen_t slen = sizeof(addr_server);

    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        error("socket");

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons((short)atoi(argv[2])); // PORT
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
    printf("*********************\n*CONNECTED TO THE SERVER*\n*********************\n\n");
    int go = TRUE, recv_len;
    socklen_t slen = sizeof(addr_server);
    char command[MESSAGE_LEN], command_group[MESSAGE_LEN], buffer[MESSAGE_LEN], user[16];

    do
    {
        input_menu();
        get_one_line(stdin, command, MESSAGE_LEN);

        //* ---------------------------------------------------- CLIENT-SERVER --------------------------------------------------- *//

        if (!strcmp(command, "1"))
        {
            //? PEDIDO DE COMUNICAÇÃO

            char c_s_info[MESSAGE_LEN] = "";
            client_server_comm(); // menu
            get_one_line(stdin, user, 16);

            snprintf(c_s_info, strlen(username) + strlen(user) + 1, username, "User %s is asking for CLIENT/SERVER communication with user %s.\n", username, user);
            sendto(server_fd, (const char *)c_s_info, strlen(c_s_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

            //? RECEBER CONFIRMACAO
            recvfrom_nonblocking(server_fd);

            if ((recv_len = recvfrom(server_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
            {
                error("Erro no recvfrom");
            }

            buffer[recv_len] = '\0';
            printf("Confirmation received! You can start communicating with user %s.\n", username, user);

            // TODO: iniciar comunicação client->server->client

            do{
                get_one_line(stdin, c_s_info, MESSAGE_LEN);
                sendto(server_fd, (const char *) c_s_info, strlen(c_s_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));
            } while(go);
        }

        //* --------------------------------------------------------- P2P --------------------------------------------------------- *//

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
            // TODO: ter variável para IP e variável para porto

            // TODO: iniciar comunicação P2P
            // p2p_communication(ip, porto);
        }

        //* -------------------------------------------------------- GROUP --------------------------------------------------------- *//

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

        //* ---------------------------------------------------------- EXIT --------------------------------------------------------- *//

        else if (!strcmp(command, "4"))
        {
            go = FALSE;
        }

    } while (go);
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

int p2p_communication(char *user_destination_ip, char *user_destination_port)
{
    int user_destination_fd, recv_len;
    char buffer[MESSAGE_LEN];
    struct hostent *hostPtr;
    struct sockaddr_in addr_user_destination;
    socklen_t slen = sizeof(addr_user_destination);
    char endDestination[100];

    strcpy(endDestination, user_destination_ip);
    if ((hostPtr = gethostbyname(endDestination)) == 0)
        error("não consegui obter endereço");

    socklen_t slen = sizeof(addr_user_destination);

    if ((user_destination_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        error("socket");

    addr_user_destination.sin_family = AF_INET;
    addr_user_destination.sin_port = htons((short)atoi(user_destination_port));
    addr_user_destination.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;

    printf("**********\nCONNECTED TO USER %d\n**********\n", user_destination_ip);

    int recv_len;
    char message[MESSAGE_LEN];

    do{
        printf("Message: \n");
        get_one_line(stdin, message, MESSAGE_LEN);

        //* ENVIA MENSAGEM A USER
        sendto(user_destination_fd, (const char*)message, strlen(message), 0, (const struct sockaddr *)&addr_user_destination, sizeof(addr_user_destination));

        //? NÃO SEI SE ESTA PARTE É NECESSÁRIA
        recvfrom_nonblocking(user_destination_fd);

        //* RECEBE MENSAGEM DO USER
        if ((recv_len = recvfrom(user_destination_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_user_destination, (socklen_t *)&slen)) == -1)
            {
                error("Erro no recvfrom");
            }
        buffer[recv_len] = '\0';
        printf("%s\n", buffer);
    } while(TRUE);

}

