#include "global.h"
#include "client.h"

// cliente {endereço do servidor} {porto}
// ligação UDP

// diferente do TCP:
// read --> recvfrom
// write --> sendto

#define PORT 3200

pthread_t thread_id;
int fd;

/*
*-----------------------------------------------------------------------------------------------------------------------------------
*--------------------------------------------------------------- MAIN --------------------------------------------------------------
*-----------------------------------------------------------------------------------------------------------------------------------
*/
int main(int argc, char *argv[])
{

    // ------------------ DECLARAÇÕES VARIÁVEIS ------------------ //

    int recv_len;
    char buffer[MESSAGE_LEN];
    struct hostent *hostPtr;
    struct sockaddr_in addr_server;
    socklen_t slen = sizeof(addr_server);
    char endServer[100];

    // ------------------ EXECUTAR PROGRAMA CLIENTE ------------------ //

    execute_client(argc, argv, endServer, hostPtr, addr_server, fd);

    // ------------------ AUTENTIFICAÇÃO ------------------ //

    char username[16], password[16];

    while (authentication(fd, addr_server, username, password) == 0)
        ;

    printf("*********************\nCONNECTED TO THE SERVER\n*********************\n\n");

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
    if (argv[1] != IP_SERVER)
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

int authentication(int fd, struct sockaddr_in addr_server, char *username, char *password)
{
    int recv_len;
    char buffer[MESSAGE_LEN];
    struct sockaddr_in addr_server;
    socklen_t slen = sizeof(addr_server);

    while (1)
    {
        printf("Username: ");
        if (!fgets(username, sizeof(username), stdin))
        {
            printf("Formato errado do username. Tente novamente...\n");
            continue;
        }
        break;
    }

    while (1)
    {
        printf("Password: ");
        if (!fgets(password, sizeof(password), stdin))
        {
            printf("Formato errado da password. Tente novamente...\n");
            continue;
        }
        break;
    }

    char id_info[strlen(username) + strlen(password) + 1];
    snprintf(id_info, strlen(username) + strlen(password) + 1, "%s;%s;%s", LOGIN, username, password);
    sendto(fd, (const char *)id_info, strlen(id_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

    // ------------------ RECEBER RESPOSTA ------------------ //
    recvfrom_nonblocking(fd);

    if ((recv_len = recvfrom(fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
    {
        error("Erro no recvfrom");
        exit(1);
    }
    printf("%s", buffer);
    if (is_error(buffer))
    {
        return 0;
    }
    return 1;
}

void communication(int server_fd, struct sockaddr_in addr_server, char *username)
{
    int go = TRUE, recv_len;
    socklen_t slen = sizeof(addr_server);
    char command[MESSAGE_LEN], command_group[MESSAGE_LEN], buffer[MESSAGE_LEN], user[16];

    input_menu();
    get_one_line(stdin, command, MESSAGE_LEN);
    char c_s_info[MESSAGE_LEN];
    char message[MESSAGE_LEN - 16];

    //* ---------------------------------------------------- CLIENT-SERVER --------------------------------------------------- *//

    if (!strcmp(command, "1"))
    {
        //! CRIAR THREAD AQUI
        pthread_create(&thread_id, NULL, chat, NULL);
        do
        {
            //? PEDIDO DE COMUNICAÇÃO
            strcpy(c_s_info, "");
            strcpy(message, "");
            char exit[] = "EXIT!";

            printf("Type: <user_id_destination> or <EXIT!> to quit\n");
            get_one_line(stdin, user, 16);
            if (strcmp(user, exit) == 0)
            {
                break;
            }

            printf("Type: <message> or <EXIT!> to quit\n");
            get_one_line(stdin, message, MESSAGE_LEN - 16);
            if (strcmp(message, exit) == 0)
            {
                break;
            }

            snprintf(c_s_info, MESSAGE_LEN, "%s;%s;%s", SEND_MESSAGE, user, message);
            sendto(server_fd, (const char *)c_s_info, strlen(c_s_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

            //? RECEBER CONFIRMACAO
            recvfrom_nonblocking(server_fd);

            if ((recv_len = recvfrom(server_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
            {
                error("Erro no recvfrom");
            }
            buffer[recv_len] = '\0';
            printf("%s", buffer);
            if (is_error(buffer))
            {
                error("CLIENT/SERVER");
                break;
            }

        } while (1);
    }

    //* --------------------------------------------------------- P2P --------------------------------------------------------- *//

    else if (!strcmp(command, "2"))
    {
        //? PEDIDO DE COMUNICAÇÃO

        char p2p_info[MESSAGE_LEN] = "";

        printf("Type: <user_id_destination>\n");
        get_one_line(stdin, user, 16);

        snprintf(p2p_info, strlen(username), "%s;%d", REQUEST_P2P, username);
        sendto(server_fd, (const char *)p2p_info, strlen(p2p_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

        //? RECEBER ENDEREÇO E PORTO UDP

        recvfrom_nonblocking(server_fd);

        if ((recv_len = recvfrom(server_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
        {
            error("Erro no recvfrom");
        }

        buffer[recv_len] = '\0';

        printf("%s", buffer);
        if (is_error(buffer)){
            error("P2P");
            break;
        }

        printf("******\nReceived UDP address and port: %s\n******\n", buffer);

        //! CRIAR THREAD AQUI
        pthread_create(&thread_id, NULL, chat, NULL);

        char *token;
        token = strtok(buffer, ";");
        char p2p_ip_destination[16] = "";
        strcpy(p2p_ip_destination, token);
        token = strtok(NULL, ";");
        char p2p_port_destination[6] = "";
        strcpy(p2p_port_destination, token);

        //? voltar a criar config

        struct hostent *hostPtr;

        if ((hostPtr = gethostbyname(p2p_ip_destination)) == 0)
            error("não consegui obter endereço");

        struct sockaddr_in addr_p2p;
        addr_p2p.sin_family = AF_INET;
        addr_p2p.sin_port = htons((short)atoi(p2p_port_destination));
        addr_p2p.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;

        do
        {
            char exit[] = "EXIT!";
            printf("Type a message: <message> to send or <EXIT!> to quit\n");
            get_one_line(stdin, message, MESSAGE_LEN - 16);
            if (strcmp(message, exit) == 0)
            {
                break;
            }
            sendto(server_fd, (const char *)message, strlen(message), 0, (const struct sockaddr *)&addr_p2p, sizeof(addr_p2p));
        } while (1);
    }

    //* -------------------------------------------------------- GROUP --------------------------------------------------------- *//

    else if (!strcmp(command, "3"))
    {
        char group_info[MESSAGE_LEN];
        char group_name[GROUP_NAME_LEN];

        //? PEDIDO DE COMUNICAÇÃO

        strcpy(group_info, "");
        strcpy(group_name, "");
        printf("Type: <group_name> you wanna create\n");
        get_one_line(stdin, group_name, GROUP_NAME_LEN);

        snprintf(group_info, strlen(group_info), "%s;%s", CREATE_GROUP, group_name);
        sendto(server_fd, (const char *)group_info, strlen(group_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

        recvfrom_nonblocking(server_fd);

        //? RECEBER ENDEREÇO MULTICAST

        if ((recv_len = recvfrom(server_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
        {
            error("Erro no recvfrom");
        }

        buffer[recv_len] = '\0';

        //! CRIAR THREAD AQUI
        pthread_create(&thread_id, NULL, chat, NULL);

        // TODO: is error
        printf("Received group multicast address: %s\n", buffer);
    }

    else if (!strcmp(command_group, "4"))
    {
        char group_info[MESSAGE_LEN];
        char group_name[GROUP_NAME_LEN];

        //? PEDIDO DE COMUNICAÇÃO

        strcpy(group_info, "");
        strcpy(group_name, "");
        printf("Type: <group_name> you want to access\n");
        get_one_line(stdin, group_name, GROUP_NAME_LEN);

        snprintf(group_info, strlen(group_info), "%s:%s", ACCESS_GROUP, group_name);
        sendto(server_fd, (const char *)group_info, strlen(group_info), 0, (const struct sockaddr *)&addr_server, sizeof(addr_server));

        recvfrom_nonblocking(server_fd);

        //? RECEBER O ENDEREÇO MULTICAST A USAR

        if ((recv_len = recvfrom(server_fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr_server, (socklen_t *)&slen)) == -1)
        {
            error("Erro no recvfrom");
        }

        buffer[recv_len] = '\0';

        if()

        // TODO: is error
        printf("Received group multicast address and port: %s\n", buffer); // multicast_adress; multicast_port

        //! CRIAR THREAD AQUI
        pthread_create(&thread_id, NULL, chat, &addr_server);

        char *token;
        token = strtok(buffer, ";");
        char group_ip_destination[16] = "";
        strcpy(group_ip_destination, token);
        token = strtok(NULL, ";");
        char group_port_destination[6] = "";
        strcpy(group_port_destination, token);

        //? iniciar comunicação grupo

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(group_ip_destination);
        addr.sin_port = htons(group_port_destination);

        int multicastTTL = 255;
        if (setsockopt(server_fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&multicastTTL,
                       sizeof(multicastTTL)) < 0)
        {
            perror("socket opt");
            return 1;
        }

        do
        {
            char exit[] = "EXIT!";
            strcpy(message, "");
            printf("Type: <message> to send to group or <EXIT!> to quit\n");
            get_one_line(stdin, message, MESSAGE_LEN - 16);
            if (strcmp(message, exit) == 0)
            {
                break;
            }
            int nbytes = sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&addr, sizeof(addr));

            if (nbytes < 0)
            {
                perror("sendto");
                return 1;
            }
        } while (1);
    }

    //* ---------------------------------------------------------- EXIT --------------------------------------------------------- *//

    else if (!strcmp(command, "5"))
    {
        int nbytes = sendto(server_fd, DISCONNECT, strlen(DISCONNECT), 0, (struct sockaddr *)&addr_server, sizeof(addr_server));

        if (nbytes < 0)
        {
            perror("sendto");
            return 1;
        }
    }

    pthread_join(thread_id, NULL);
    printf("\nLeaving server communication...\n");
    return 0;
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

void error(char *msg)
{
    printf("Error: %s\n", msg);
    exit(-1);
}

int is_error(char *string)
{
    int len_error = strlen(ERROR);

    if (len_error > strlen(string))
        return 0;
    char error[strlen(ERROR)];
    strcpy(error, ERROR);
    for (unsigned int i = 0; i < len_error; i++)
    {
        if (error[i] != string[i])
            return 0;
    }
    return 1;
}

void input_menu()
{
    printf("Choose a type of communication [1 - 3]:\n"
           "1 - Client/Server\n"
           "2 - P2P\n"
           "3 - Create group\n"
           "4 - Join group\n"
           "5 - Exit\n");
}

void *chat()
{
    struct sockaddr_in addr;
    int recv_len;
    socklen_t slen = sizeof(addr);
    char buffer[MESSAGE_LEN];

    do
    {
        recvfrom_nonblocking(fd);
        if ((recv_len = recvfrom(fd, buffer, MESSAGE_LEN, 0, (struct sockaddr *)&addr, (socklen_t *)&slen)) == -1)
        {
            error("Erro no recvfrom");
        }

        buffer[MESSAGE_LEN] = '\0';

        int hours, minutes, seconds;
        time_t now = time(NULL);

        struct tm *local = localtime(&now);
        hours = local->tm_hour;
        minutes = local->tm_min;
        seconds = local->tm_sec;

        printf("** MESSAGE RECEIVED **\n%02d:%02d:%02d : %s\n\n", hours, minutes, seconds, buffer);
    } while (1);
}