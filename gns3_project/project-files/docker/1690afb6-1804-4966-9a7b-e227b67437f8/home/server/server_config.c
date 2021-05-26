#include "server.h"
#include <errno.h>
#include <string.h>

void process_config(struct sockaddr_in config_addr);
void add_client(char *token);
void delete_client(char *token);
void list_clients();
int fd_config_server, fd_config;

void send_tcp_message(char *message, int fd)
{
    write(fd, message, 1 + strlen(message));
}
void sigint_handler(int signum)
{
    printf("SERVER CONFIG IS CLOSING\n");
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
void server_config(char *port_config) //TODO: RECEIVE CLIENTS FILE
{
    signal(SIGINT, sigint_handler);
    /*************************TCP****************************/
    printf("SERVER CONFIG\n");
    struct sockaddr_in addr_server_config, config_addr;
    int config_addr_size = sizeof(config_addr);

    char end_server[INET_ADDRSTRLEN] = "";
    strcpy(end_server, IP_SERVER_PRIVATE);
    int fd_config_server;
    struct hostent *hostPtr;

    if ((hostPtr = gethostbyname(end_server)) == 0)
        perror("Nao consegui obter endereço");

    bzero((void *)&addr_server_config, sizeof(addr_server_config));

    addr_server_config.sin_family = AF_INET;
    addr_server_config.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr_server_config.sin_port = htons(atoi(port_config));

    if ((fd_config_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("Socket");

    if (bind(fd_config_server, (struct sockaddr *)&addr_server_config, sizeof(addr_server_config)) < 0)
        perror("Bind:");

    if (listen(fd_config_server, 5) < 0)
        perror("Listen");

    fd_config = accept(fd_config_server, (struct sockaddr *)&config_addr, (socklen_t *)&config_addr_size);
    if (fd_config > 0)
    {
        process_config((struct sockaddr_in)config_addr);
        close(fd_config);
    }
    close(fd_config_server);
    printf("SERVER  CONFIG IS CLOSING\n");
    return;
}

void process_config(struct sockaddr_in config_addr) 
{
    int nread = 0;
    char command[MESSAGE_LEN - 1];
    char message[MAX_LINE - 1];
    int go = FALSE;
    /*int counter;
    
    char admin_user[20] = "admin1234";
    char admin_password[20]="12345";

    //TODO: LOGIN ADMIN

    nread = read(fd_config, command, MESSAGE_LEN - 1);
    command[nread] = '\0';

    char *token;
    printf("--->received: %s\n", command);
    token = strtok(command, DELIM);
    if(token ==NULL || strcmp(token, "LOGIN") != 0){
        strcpy(message, "ERROR:invalid registration");
        send_tcp_message(message, fd_config);
        return;
    }
    token = strtok(NULL, DELIM); // LOGIN

    token = strtok(NULL, DELIM); // username
    char valid_user[SIZE];
    strcpy(valid_user, token);
    printf("USER_ADMIN: %s\n", valid_user);

    token = strtok(NULL, DELIM); // password
    char valid_password[SIZE];
    strcpy(valid_password, token);
    printf("USER PASSWORD: %s\n", valid_password);

    if (strcmp(valid_password, admin_password) == 0 && strcmp(valid_user, admin_user)==0){
        printf("Valid user and password\n");
        go = TRUE;
        
    }

    printf("Wrong admin user or password. Try again...\n");
    */


    go = TRUE;
    do
    {
        nread = read(fd_config, command, MESSAGE_LEN - 1);
        command[nread] = '\0';
        strcpy(message, command);
        remove_end_line(command);
        if (!strcmp(command, "QUIT"))
        {
            printf("QUIT command received\n");
            go = FALSE;
        }
        else if (!strcmp(command, "LIST"))
        {
            printf("LIST command received\n");
            list_clients();
        }
        else
        {
            char *token;
            token = strtok(command, DELIM);
            if (!strcmp(token, "ADD"))
            {
                printf("ADD command received\n");
                add_client(message);
            }
            else if (!strcmp(token, "DEL"))
            {
                printf("DEL command received\n");
                delete_client(token);
            }
            else
            {
                send_tcp_message("Error: invalid Command", fd_config);
            }
        }
    } while (go == TRUE && nread > 0);
    fflush(stdout);
    close(fd_config);
}

void list_clients()
{
    sem_wait(&mutex_registers);
    FILE *file = fopen(clients_file, "rb");
    user_t user;
    int nread;
    char send[MESSAGE_LEN];
    char received[MESSAGE_LEN];
    while (fread(&user, sizeof(user_t), 1, file))
    {
        convert_user_struct_in_string(user, send);
        send_tcp_message(send, fd_config);
        nread = read(fd_config, received, MESSAGE_LEN - 1);
        if (nread < 0)
            break;
        received[nread] = '\0';
        if (strcmp(received, "ERROR") == 0)
        {
            printf("Received error message from admin\n");
            break;
        }
    }
    send_tcp_message("FINAL", fd_config);
    fclose(file);
    sem_post(&mutex_registers);
    printf("<<list clients");
}
void add_client(char *token)
{
    user_t user = convert_string_to_user(token);
    if (is_null_user(user))
    {
        send_tcp_message("Invalid arguments\nError adding the client", fd_config);
        return;
    }
    printf(">>after null_user\n");
    if (add_to_file(user))
        send_tcp_message("User added successfully", fd_config);
    else
        send_tcp_message("Error adding the user", fd_config);
}
void delete_client(char *token)
{
    char username[20];
    token = strtok(NULL, DELIM);
    strcpy(username, token);

    if (delete_from_file(username))
        send_tcp_message("Client successfully removed", fd_config);
    else
        send_tcp_message("Error while removing the user", fd_config);
}
