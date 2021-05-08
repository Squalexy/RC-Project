
#include "server_config.h"

//para tratar da administracao do servidor
void server_config(char *port_config)
{
    /*************************TCP****************************/
    struct sockaddr_in addr_server_config, config_addr;
    char *endServer;
    int config_addr_size = sizeof(config_addr);
    strcpy(endServer, IP_SERVER);
    int fd_config_server, config;
    struct hostent *hostPtr;

    if ((hostPtr = gethostbyname(endServer)) == 0)
        erro("Nao consegui obter endereço");

    bzero((void *)&addr_server_config, sizeof(addr_server_config));

    addr_server_config.sin_family = AF_INET;
    addr_server_config.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr_server_config.sin_port = htons((short)atoi(port_config));

    if ((fd_config_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("socket");

    if (bind(fd_config_server, (struct sockaddr *)&addr_server_config, sizeof(addr_server_config)) < 0)
        error("bind");

    if (listen(fd_config_server, 5) < 0)
        error("listen");

    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    //wait for new connection
    config = accept(fd_config_server, (struct sockaddr *)&config_addr, (socklen_t *)&config_addr_size);
    if (config > 0)
    {
        process_config(config, (struct sockaddr_in)config_addr);
        close(config);
        exit(0);
    }
    close(fd_config_server);
}
void process_config(int fd_config, struct sockaddr_in config_addr)
{
    int nread = 0;
    char command[MESSAGE_LEN];
    char send[MESSAGE_LEN];
    int go = TRUE;

    do
    {

        nread = read(fd_config, command, MESSAGE_LEN - 1);
        command[nread] = '\0';
        if (!strcmp(command, "QUIT"))
            go = FALSE;
        else if (!strcmp(command, "LIST"))
        {
            printf("list users\n");
        }
        else
        {
            char *token;
            const char delim[2] = " ";
            token = strtok(command, delim);
            if (!strcmp(token, "ADD"))
            {
                if (add_client(token, delim) == 0)
                {
                    strcpy(send, "Continue");
                }
                else
                {
                    strcpy(send, "Error");
                }
            }
            else if (!strcmp(token, "DEL"))
            {
                char *userId;
                token = strtok(NULL, delim);
                strcpy(userId, token);
                delete_from_file(userId);
            }
            else
            {
                //invalid command
            }
        }
        write(fd_config, send, 1 + strlen(send));
    } while (go = TRUE && nread > 0);
}

int add_client(char *token, const char delim[2])
{
    char *string;
    strcpy(string, token); //!!!!!!!!!!!!!VER ISTO
    user_t *user = convert_to_user_struct(string);
    add_register(user);
}