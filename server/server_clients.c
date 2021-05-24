
#include "server.h"
//************************ HANDLES CLIENTS REQUESTS ************************//
// variables
int fd_server;
struct sockaddr_in addr_client;
int nclients_activate;
user_t active_clients[MAX_CLIENTS];
int number_groups;

// handle communications
void login_user(char *token);
int validate_communication(int type_communication, user_t user);
void p2p_request(char *token, user_t user);
void client_server_request(char *token, user_t user);
void group_acces_request(char *token, user_t user);
void create_multicast_group(char *token, user_t user);
void disconnect_client(user_t user, int pos);

// groups file
int add_group(user_t user, char *address, short port, char *group_name);
int find_group_in_file(char *group_name, group_t *group);
int add_group_to_file(group_t group);
int count_groups();

// send messages
void send_message(struct sockaddr_in addr, char *format, ...);
void send_error(char *type);

//registrations list
int find_user(char *ip, user_t *user);
void delete_user(int pos);
void handle_sigint();

void server_to_clients(char *port_clients)
{
    nclients_activate = 0;
    number_groups = count_groups();
        // ************************ UDP ************************* //

    printf("Server for clients [%d]\n", getpid());
    int fd_server;
    struct sockaddr_in addr_server, addr_client;
    socklen_t client_len = sizeof(addr_client);

    if ((fd_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        perror("Error in socket\n");

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port_clients);
    addr_server.sin_addr.s_addr = htonl(IP_SERVER_PRIVATE); // !!!!!! CHECK IF

    if (bind(fd_server, (struct sockaddr *)&addr_server, sizeof(addr_server)) == -1)
        perror("Error in bind");

    signal(SIGINT, handle_sigint);
    char buffer[MESSAGE_LEN];
    char *token;
    int rclen;

    while (TRUE)
    {
        if ((rclen = recvfrom(fd_server, buffer, MESSAGE_LEN - 1, 0, (struct sockaddr *)&addr_client, (socklen_t *)&client_len)) == -1)
            perror("Error in recvfrom");

        buffer[rclen] = '\0';

        token = strtok(buffer, DELIM);
        if (!strcmp(token, LOGIN))
        {
            login_user(token);
            continue;
        }
        user_t user;
        // search user in the list of connected users
        int pos;
        if ((pos = find_user(inet_ntoa(addr_client.sin_addr), &user)) < 0)
        {
            send_error("Needs login before");
            continue;
        }

        if (!strcmp(token, REQUEST_P2P))
            p2p_request(token, user);
        else if (!strcmp(token, SEND_MESSAGE))
            client_server_request(token, user);
        else if (!strcmp(token, ACCESS_GROUP))
            group_acces_request(token, user);
        else if (!strcmp(token, CREATE_GROUP))
            create_multicast_group(token, user);
        else if (!strcmp(token, DISCONNECT))
            disconnect_client(user, pos);
    }
}

/**
 * @brief function to handle the user's login
 * 
 * @param token 
 */
void login_user(char *token)
{
    printf("LOGIN\n");
    char user_id[SIZE];
    char password[SIZE];

    token = strtok(NULL, DELIM);
    strcpy(user_id, token);
    token = strtok(NULL, DELIM);
    strcpy(password, token);

    user_t *user = search_user(user);

    if (user != NULL && strcmp(user->password, password) == 0)
    {
        send_message(addr_client, "COMMUNICATIONS:\nClient-Server: %s; P2P: %s; Group: %s", user->client_server, user->p2p, user->group);
    }

    if (nclients_activate + 1 <= MAX_CLIENTS)
    {
        active_clients[nclients_activate] = (*user);
        nclients_activate++;
    }
    free(user);
    send_error("Invalid registration");
}

/**
 * @brief see if a user is authorized to make a certain type of communication
 * 
 * @param type_communication the type of communication (1 «=» client-server, 2 «=» P2P, 3 «=» group)
 * @param user Structure of the user who made the request
 * @return int 1 if authorizeed, 0 otherwise
 */
int validate_communication(int type_communication, user_t user)
{
    int return_value;
    switch (type_communication)
    {
    case 1: //client-server
        return_value = !strcmp(user.client_server, "yes");
        break;
    case 2: //p2p
        return_value = !strcmp(user.p2p, "yes");
    case 3: // group
        return_value = !strcmp(user.group, "yes");
    }
    return return_value;
}

/**
 * @brief Function to handle the P2P request
 * Validates the received information and if there is no problem sends to the client the Ip and port that he requested
 * In case of any error, send theh clients a error message
 * @param token Token withe the information about the request
 * @param user Structure of the user who made the request
 */
void p2p_request(char *token, user_t user)
{
    int type = 2;
    char user_id[SIZE];
    //checks if the user is authorized to make that type of communications
    if (validate_communication(2, user) == 0)
    {
        send_error("You are not authorized to this type of communication");
        return;
    }
    token = strtok(NULL, DELIM);
    if (token == NULL)
    {
        send_error("Invalid request format");
        return;
    }
    strcpy(user_id, token);
    user_t *dest = search_user(user_id);
    //checks if the destination clients is authorized to that type of communications
    if (validate_communication(2, (*dest)) == 0)
    {
        send_error("Destination client is not authorized to do this type of communication");
        return;
    }

    if (dest == NULL)
        send_error("User not found");
    else
    {
        send_message(addr_client, "%s;%s", dest->ip, dest->port);
    }
    free(dest);
}

/**
 * @brief Function to handle the client-server request
 * Validates the information about the clients (origin and destination) and if there is no problem sends the message to the destination client
 * otherwise sends a erros message to the origin client 
 * 
 * @param token Token withe the information about the request
 * @param user Structure of the user who made the request
 */
void client_server_request(char *token, user_t user)
{
    //checks if the user is authorized to make that type of communications
    if (validate_communication(1, user) == 0)
    {
        send_error("You are not authorized to this type of communication");
        return;
    }
    char message[MESSAGE_LEN];
    char user_id[SIZE];
    token = strtok(NULL, DELIM);
    if (token == NULL)
    {
        send_error("Invalid request format");
        return;
    }
    strcpy(user_id, token);
    //checks if the destination clients is authorized to that type of communications
    user_t *dest = search_user(user_id);

    if (validate_communication(1, (*dest)) == 0)
    {
        send_error("Destination client is not authorized to do this type of communication");
        return;
    }

    token = strtok(NULL, DELIM);
    if (token == NULL)
    {
        send_error("Invalid request format");
        return;
    }
    strcpy(message, token);

    struct sockaddr_in destination_client;
    memset(&destination_client, 0, sizeof(destination_client));
    struct hostent *hostPTR;
    if ((hostPTR = gethostbyname(dest->ip)) == 0)
    {
        send_error("Sending the message");
        return;
    }
    destination_client.sin_family = AF_INET;
    destination_client.sin_port = htons((short)atoi(dest->port)); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECK THIS
    destination_client.sin_addr.s_addr = ((struct in_addr *)(hostPTR->h_addr))->s_addr;

    send_message(destination_client, message);

    send_message(addr_client, "MEESSAGE SEND TO DESTINATION");
}

/**
 * @brief 
 * 
 * @param token 
 * @param user Structure of the user who made the request
 */
void group_acces_request(char *token, user_t user)
{
    //checks if the user is authorized to make that type of communications
    if (validate_communication(3, user) == 0)
    {
        send_error("You are not authorized to this type of communication");
        return;
    }

    char group_name[SIZE];
    token = strtok(NULL, DELIM);
    if (token == NULL)
    {
        send_error("Invalid request format");
        return;
    }
    strcpy(group_name, token);

    group_t group;
    if (find_group_in_file(group_name, &group) == 0)
    {
        send_error("Group not found");
        return;
    }
    send_message(addr_client, "%s;%s;%s", group.group_name, group.multicast_address, group.port);
}

/**
 * @brief Create a multicast group object
 * 
 * @param token 
 * @param user Structure of the user who made the request
 */
void create_multicast_group(char *token, user_t user)
{
    //checks if the user is authorized to make that type of communications
    if (validate_communication(3, user) == 0)
    {
        send_error("You are not authorized to this type of communication");
        return;
    }

    char group_name[SIZE];
    token = strtok(NULL, DELIM);
    if (token == NULL)
    {
        send_error("Invalid request format");
        return;
    }
    strcpy(group_name, token);
    group_t new_group;
    if (find_group_in_file(group_name, &new_group) == 1)
    {
        send_error("Already a group with this name");
        return;
    }

    if (number_groups + 1 >= MAX_GROUPS)
    {
        send_error("Impossible to create a new group.");
        return;
    }
    //new_group.owner = user;
    strcpy(new_group.multicast_address, mult_address[number_groups]);
    strcpy(new_group.group_name, group_name);
    number_groups++;
    add_group_to_file(new_group);
    send_message(addr_client, "%s", new_group.multicast_address);
}
/**
 * @brief Deletes a client from the list of activated clients
 * 
 * @param user Structure of the user who made the request
 * @param pos Position in the list of activated clients 
 */
void disconnect_client(user_t user, int pos)
{
    delete_user(pos);
}
// ****************************** GROUP FILE ****************************** //

/**
 * @brief Searches for a certain group in the file
 * 
 * @param group_name Groups name to search
 * @param group Pointer to the group struture where to store the result
 * @return int 1 if the group was found, 0 otherwise
 */
int find_group_in_file(char *group_name, group_t *group)
{
    FILE *file = fopen(GROUPS_FILE, "rb");
    if (file == NULL)
        return 0;

    while (fread(group, sizeof(group_t), 1, file))
    {
        if (strcmp(group->group_name, group_name) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}
/**
 * @brief Adds a new group to the groups file
 * 
 * @param group The group structure to add
 * @return int 1 if all goes well, 0 otherwise
 */
int add_group_to_file(group_t group)
{
    group_t *aux;
    if (find_in_file(group.group_name, &aux))
        return 0;

    FILE *file = fopen(GROUPS_FILE, "ab");
    fwrite(&group, sizeof(group_t), 1, file);
    printf("Group was added\n");
    fclose(file);
    return 1;
}

int count_groups()
{
    FILE *file = fopen(GROUPS_FILE, "rb");
    if (file == NULL)
        return 0;
    fseek(file, 0, SEEK_END);
    long count = (long)(ftell(file) / sizeof(user_t));
    fclose(file);
    return (int)count;
}
// ****************************** SEND MESSAGES ****************************** //

/**
 * @brief Function to send messages using UDP with a certain format
 * 
 * @param format The format of the message to send
 * @param ... Arguments of the message
 */
void send_message(struct sockaddr_in addr, char *format, ...)
{
    char send[MESSAGE_LEN];
    va_list arg;
    va_start(arg, format);
    vsprintf(send, format, arg);
    va_end(arg);
    send_to(fd_server, (const char *)send, strlen(send), 0, (struct sockaddr *)&addr, sizeof(addr));
}

/**
 * @brief Function to send error messages to the client
 * 
 * @param type Type of error
 */
void send_error(char *type)
{
    char send[MESSAGE_LEN] = "ERROR: ";
    strcat(send, type);
    send_to(fd_server, (const char *)send, strlen(send), 0, (struct sockaddr *)&addr_client, sizeof(addr_client));
}

// ****************************** REGISTRATIONS LIST ******************************

int find_user(char *ip, user_t *user)
{
    for (int i = 0; i < nclients_activate; i++)
    {
        if (strcmp(active_clients[i].ip, ip) == 0)
        {
            (*user) = active_clients[i];
            return i;
        }
    }
    return -1;
}

void delete_user(int pos)
{
    for (int i = pos + 1; i < nclients_activate; i++)
    {
        active_clients[pos - 1] = active_clients[pos];
    }
    nclients_activate--;
}

void handle_sigint()
{
    close(fd_server);
    printf("CLIENTS SERVER IS CLOSING\n");
    exit(0);
}