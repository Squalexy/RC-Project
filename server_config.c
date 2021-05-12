
#include "server_config.h"
#include <errno.h>
#include <string.h>

void process_config(int fd_config, struct sockaddr_in config_addr);
void add_client(char *token, int fd_config);
void delete_client(char *token, int fd_config);
void list_clients(int fd_config);
int fd_config_server, fd_config;
void send_message(char *message, int fd)
{
    strcat(message, "\n");
    write(fd, message, 1 + strlen(message));
}
void sigint_handler(int signum)
{
    printf("server is closing\n");
    close(fd_config);
    close(fd_config_server);
    close_mutex_registers();
    exit(0);
}
void remove_end_line(char *string)
{
    while (*string && *string != '\n' && *string != '\r')
        string++;

    *string = '\0';
}
void server_config(char *port_config)
{
    signal(SIGINT, sigint_handler);
    /*************************TCP****************************/
    printf("SERVER CONFIG\n");
    struct sockaddr_in addr_server_config, config_addr;
    int config_addr_size = sizeof(config_addr);

    /**strcpy(endServer, IP_SERVER);
    int fd_config_server, config, port;
    struct hostent *hostPtr;

    if ((hostPtr = gethostbyname(endServer)) == 0)
       perror("Nao consegui obter endereço");
    */
    bzero((void *)&addr_server_config, sizeof(addr_server_config));

    addr_server_config.sin_family = AF_INET;
    addr_server_config.sin_addr.s_addr = htonl(INADDR_ANY); //((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr_server_config.sin_port = htons(atoi(port_config));

    if ((fd_config_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("socket");

    if (bind(fd_config_server, (struct sockaddr *)&addr_server_config, sizeof(addr_server_config)) < 0)
        perror("BIND:");

    if (listen(fd_config_server, 5) < 0)
        perror("listen");

    fd_config = accept(fd_config_server, (struct sockaddr *)&config_addr, (socklen_t *)&config_addr_size);
    if (fd_config > 0)
    {
        process_config(fd_config, (struct sockaddr_in)config_addr);
        close(fd_config);
    }
    close(fd_config_server);
    printf("SERVER IS CLOSING\n");
    return 0;
}

void process_config(int fd_config, struct sockaddr_in config_addr)
{
    printf("Process config\n");
    int nread = 0;
    char command[MESSAGE_LEN - 1];
    int go = TRUE;

    do
    {
        nread = read(fd_config, command, MESSAGE_LEN - 1);
        command[nread] = '\0';
        remove_end_line(command);
        printf("----%s****\n", command);
        if (!strcmp(command, "QUIT"))
        {
            go = FALSE;
            printf("quit command\n");
        }
        else if (!strcmp(command, "LIST"))
        {
            list_clients(fd_config);
        }
        else
        {
            char *token;
            token = strtok(command, DELIM);
            if (!strcmp(token, "ADD"))
            {
                add_client(token, fd_config);
            }
            else if (!strcmp(token, "DEL"))
            {
                delete_client(token, fd_config);
            }
            else
            {
                send_message("Error: invalid Command", fd_config);
            }
        }
    } while (go == TRUE && nread > 0);
    close(fd_config);
}

void add_client(char *token, int fd_config)
{
    printf(">>add client\n");
    char string[MESSAGE_LEN];
    char message[MESSAGE_LEN];
    printf("**************\n");
    strcpy(string, token); //!!!!!!!!!!!!!VER ISTO
    //token = strtok(NULL, DELIM);
    printf("!!!!!!!!!!!!!!\n");
    user_t *user = convert_to_user_struct(string);
    if (user != NULL && add_register(user))
    {
        snprintf(message, MESSAGE_LEN - 1, "Client %s added successfully", user->user_id);
    }
    else
        snprintf(message, MESSAGE_LEN - 1, "Error adding client %s", user->user_id);
    send_message(message, fd_config);
    printf("<<end add_client\n");
}
void delete_client(char *token, int fd_config)
{
    char *userId = NULL;
    char message[MESSAGE_LEN - 1];
    token = strtok(NULL, DELIM);
    strcpy(userId, token);
    if (delete_from_file(userId))
    {
        snprintf(message, MESSAGE_LEN - 1, "Client %s successfully removed", userId);
    }
    else
        snprintf(message, MESSAGE_LEN - 1, "Error removing client %s", userId);
    send_message(message, fd_config);
}

void list_clients(int fd_config)
{
    char message[MESSAGE_LEN - 1];
    sem_wait(&mutex_registers);
    FILE *registers = fopen(CLIENTS_FILE, "rb");
    if (registers == NULL)
    {
        strcpy(message, "Error in server");
        send_message(message, fd_config);
        sem_post(&mutex_registers);
        return;
    }
    long total_clients = count_clients(registers);
    snprintf(message, MESSAGE_LEN - 1, "%ld CLIENTS", total_clients + 1);
    send_message(message, fd_config);
    snprintf(message, MESSAGE_LEN - 1, "User Id||IP||Password||Client-server||P2P||Group");
    send_message(message, fd_config);
    user_t user;
    for (int i = 0; i < total_clients; i++)
    {
        fread(&user, sizeof(user_t), 1, registers);
        snprintf(message, MESSAGE_LEN - 1, "%s||%s||%s||%s||%s||%s", user.user_id, user.ip, user.password, user.client_server, user.p2p, user.group);
        send_message(message, fd_config);
    }
    sem_post(&mutex_registers);
}
