
#include "server_clients.h"
//para tratar dos clientes

void validate_user(char *token);

int fd_server;
struct sockaddr_in addr_client;
void server_to_clients(char *port_clients)
{
    /*************************UDP****************************/
    printf("Server to clients [%d]\n", getpid());
    int fd_server;
    struct sockaddr_in addr_server, addr_client;
    socklen_t client_len = sizeof(addr_client);

    if ((fd_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        perror("Error in socket\n");

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port_clients);
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd_server, (struct sockaddr *)&addr_server, sizeof(addr_server)) == -1)
        perror("Error in bind");
    char buffer[MESSAGE_LEN];
    char *token;
    int rclen;
    while (TRUE)
    {
        if ((rclen = recvfrom(fd_server, buffer, MESSAGE_LEN - 1, 0, (struct sockaddr *)&addr_client, (socklen_t *)&client_len)) == -1)
            perror("Error in recvfrom");
        buffer[rclen] = '\0';

        token = strtok(buffer, DELIM);

        if (strcmp(token, "LOGIN"))
        {
            validate_user(token);
        }
    }
}
void handle_user_login(user_t *user);
void invalid_user();
void validate_user(char *token)
{
    char user[20];
    char password[20];

    token = strtok(NULL, DELIM);
    strcpy(user, token);
    token = strtok(NULL, DELIM);
    strcpy(password, token);

    user_t *user = search_user(user);

    if (user != NULL && strcmp(user->password, password) == 0) //se ele encontrou e a palavra pass e valida
    {
        handle_user_login(user);
    }
    invalid_user();
}

void handle_user_login(user_t *user)
{
    char message[MESSAGE_LEN];
    snprintf(message, MESSAGE_LEN - 1, "COMMUNICATIONS:\nClient-Server: %s; P2P: %s; Group: %s", user->client_server, user->p2p, user->group);
    sendto(fd_server, (const char *)message, strlen(message), 0, (struct sockaddr *)&addr_client, sizeof(addr_client));

    //TODO: add user to a list
}
void invalid_user()
{
    char message[MESSAGE_LEN] = "ERROR: Invalid registration";
    sendto(fd_server, (const char *)message, strlen(message), 0, (struct sockaddr *)&addr_client, sizeof(addr_client));
}

