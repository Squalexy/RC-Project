#include "server.h"

const char *mult_address[] = {"224.0.0.1", "224.0.0.2", "224.0.0.3", "224.0.0.4", "224.0.0.5", "224.0.0.6", "224.0.0.7", "224.0.0.8", "224.0.0.9", "224.0.0.10", "224.0.0.11", "224.0.0.12", "224.0.0.13", "224.0.0.14", "224.0.0.15", "224.0.0.16"};

//************************ HANDLES CLIENTS REQUESTS ************************//
// variables
int fd_server;

int nclients_activate;
user_t active_clients[MAX_CLIENTS];
int number_groups;
struct sockaddr_in addr_client;
// handle communications
void login_user(char *token);
int validate_communication(int type_communication, user_t user);
void p2p_request(char *token, user_t user);
void client_server_request(char *token, user_t user);
void group_acces_request(char *token, user_t user);
void create_multicast_group(char *token, user_t user);
void disconnect_client(user_t user, int pos);
void print_login_clients();
// groups file
int find_group_in_file(char *group_name, group_t *group);
int add_group_to_file(group_t group);
int count_groups();

// send messages
void send_message(struct sockaddr_in addr, char * send);
void send_error(char *type);
void recvfrom_nonblocking(int fd);

//registrations list
int find_user(char *ip, user_t *user);
void delete_user(int pos);
void handle_sigint();

void server_to_clients(char *port_clients)
{
    printf("SERVER TO CLIENTS\n");
    nclients_activate = 0;
    number_groups = count_groups();
    struct sockaddr_in addr_server;
    socklen_t client_len = sizeof(addr_client);
    struct hostent *hostPtr;
    char end_server[INET_ADDRSTRLEN] = "";
    strcpy(end_server, IP_SERVER_PRIVATE);

    if ((fd_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        perror("Error in socket\n");


    if ((hostPtr = gethostbyname(end_server)) == 0)
        perror("Invalid IP");
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(atoi(port_clients));
    addr_server.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;

    
    if (bind(fd_server, (struct sockaddr *)&addr_server, sizeof(addr_server)) == -1)
        perror("Error in bind");

    signal(SIGINT, handle_sigint);
    char buffer[MESSAGE_LEN];
    char *token;
    int rclen;
    printf("COMMUNICATIONS\n");
    while (TRUE)
    {
        printf("iterate...\n");
        print_login_clients();
        recvfrom_nonblocking(fd_server);
        if ((rclen = recvfrom(fd_server, buffer, MESSAGE_LEN , 0, (struct sockaddr *)&addr_client, (socklen_t *)&client_len)) == -1)
            perror("Error in recvfrom");

        buffer[rclen] = '\0';
        //sendto(fd_server, (const char *)"FUCK THIS", strlen("FUCK THIS"), 0, (const struct sockaddr *)&addr_client, sizeof(addr_client));
        char ip_address[INET_ADDRSTRLEN];

  inet_ntop(AF_INET, &addr_client.sin_addr,ip_address, INET_ADDRSTRLEN-1);

  printf("Client connecting from (Ip:port) %s: %d says \"%s\" \n", ip_address,addr_client.sin_port, buffer);
        printf("NEW REQUEST: %s\n", buffer);
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
            printf("----->%d", pos);
            send_error("Needs login before");
            continue;
        }
        printf("TYPES OF COMMUNICATIONS\n");
        if (!strcmp(token, REQUEST_P2P))
            p2p_request(token, user);
        else if (!strcmp(token, SEND_MESSAGE))
            client_server_request(token, user);
        else if (!strcmp(token, ACCESS_GROUP)){
            printf(">>access group");
            group_acces_request(token, user);
        }
        else if (!strcmp(token, CREATE_GROUP))
            create_multicast_group(token, user);
        else if (!strcmp(token, DISCONNECT))
            disconnect_client(user, pos);
        else
            send_error("Invalid command");
        
    }
}

/**
 * @brief function to handle the user's login
 * 
 * @param token 
 */
void login_user(char *token)
{
    printf("**************LOGIN******************\n");
    char user_id[SIZE];
    char password[SIZE];

    token = strtok(NULL, DELIM);
    if (token == NULL)
    {
        send_error("Invalid request format");
        printf("error in format\n");
        return;
    }
    strcpy(user_id, token);
    token = strtok(NULL, DELIM);
    if (token == NULL)
    {
        send_error("Invalid request format");
        printf("error in format\n");
        return;
    }
    strcpy(password, token);
    printf("Going to search the user %s\n", user_id);
    user_t user;
    int result = search_user(user_id, &user);
    if(result != 0)
        printf("--->COMMUNICATIONS:\nClient-Server: %s; P2P: %s; Group: %s\n", user.client_server, user.p2p, user.group);
    if (result != 0  && strcmp(user.password, password) == 0 && nclients_activate + 1 <= MAX_CLIENTS)
    {
        active_clients[nclients_activate] = user;
        nclients_activate++;
        char send[MESSAGE_LEN];
        snprintf(send, MESSAGE_LEN, "COMMUNICATIONS:\nClient-Server: %s; P2P: %s; Group: %s", user.client_server, user.p2p, user.group);
        send_message(addr_client, send);
        //free(user);
        return;
    }
    /*
    if (nclients_activate + 1 <= MAX_CLIENTS)
    {
        active_clients[nclients_activate] = (*user);
        nclients_activate++;
    }
    */
    //free(user);
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
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECK THIS
    printf(">> validate communication %d\n ", type_communication);
    int return_value = 0;
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
    printf("<<validate communication %d", return_value);
    return return_value;
}

/**
 * @brief Function to handle the P2P request
 * Validates the received information and if there is no problem sends to the client the Ip  he requested
 * In case of any error, send theh clients a error message
 * @param token Token withe the information about the request
 * @param user Structure of the user who made the request
 */
void p2p_request(char *token, user_t user)
{
    printf(">>>P2P_request\n");
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
    user_t dest;
    int result = search_user(user_id, &dest);
    if (result == 0){
        send_error("User not found");
    }
    //checks if the destination clients is authorized to that type of communications
    if (validate_communication(2, dest) == 0)
    {
        send_error("Destination client is not authorized to do this type of communication");
        return;
    }

    
        
    else
    {
        char send[MESSAGE_LEN];
        snprintf(send, MESSAGE_LEN, "%s;%s", REQUEST_P2P, dest.ip);
        send_message(addr_client, send);
    }
    //free(dest);
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
    user_t dest;
    int result = search_user(user_id, &dest);
    if(result == 0){
        send_error("Destination client not found");
        return;
    }
    if (validate_communication(1, dest) == 0)
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
    if ((hostPTR = gethostbyname(dest.ip)) == 0)
    {
        send_error("Sending the message");
        return;
    }
    destination_client.sin_family = AF_INET;
    destination_client.sin_port = htons((short)atoi(dest.port)); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECK THIS
    destination_client.sin_addr.s_addr = ((struct in_addr *)(hostPTR->h_addr))->s_addr;

    send_message(destination_client, message);
    char send[MESSAGE_LEN];
    snprintf(send, MESSAGE_LEN, "%s;MEESSAGE SEND TO DESTINATION", SEND_MESSAGE);
    send_message(addr_client, send);
}

/**
 * @brief 
 * 
 * @param token 
 * @param user Structure of the user who made the request
 */
void group_acces_request(char *token, user_t user)
{
    printf(">>>group_acess_request\n");
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
    printf("after searching the group\n");
    char send[MESSAGE_LEN];
    snprintf(send, MESSAGE_LEN,"%s;%s", ACCESS_GROUP, group.multicast_address );
    printf("GOING TO SEND: %s\n", send);
    send_message(addr_client, send);
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
    char send[MESSAGE_LEN];
    snprintf(send, MESSAGE_LEN, "%s;%s", CREATE_GROUP, new_group.multicast_address);
    send_message(addr_client, send);
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
    group_t aux;
    if (find_group_in_file(group.group_name, &aux))
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
void send_message(struct sockaddr_in addr, char * send)
{
    printf(">>send message\n");
    /*
    char send[MESSAGE_LEN];
    va_list arg;
    va_start(arg, format);
    vsprintf(send, format, arg);
    va_end(arg);
    */
    //sendto(fd_server, (const char *)send, strlen(send), 0, (struct sockaddr *)&addr_client, sizeof(addr_client));
    sendto(fd_server, send, strlen(send), 0, (struct sockaddr *)&addr_client, sizeof(addr_client));
    printf("<<send_message\n");
}

/**
 * @brief Function to send error messages to the client
 * 
 * @param type Type of error
 */
void send_error(char *type)
{
    printf(">>send error \n");
    char send[MESSAGE_LEN] = "ERROR: ";
    strcat(send, type);
    printf("%s", send);
    sendto(fd_server, (const char *)send, strlen(send), 0, (struct sockaddr *)&addr_client, sizeof(addr_client));
    printf("<<send_error\n");
}

// ****************************** REGISTRATIONS LIST ******************************

int find_user(char *ip, user_t *user)
{
    printf(">>>find user in logins\n");
    for (int i = 0; i < nclients_activate; i++)
    {
        if (strcmp(active_clients[i].ip, ip) == 0)
        {
            (*user) = active_clients[i];
            printf("user was in logins\n");
            return i;
        }
    }
    printf("user not found in logins\n");
    return -1;
}

void delete_user(int pos)
{
    printf(">>delete user from logins\n");
    for (int i = pos + 1; i < nclients_activate; i++)
    {
        active_clients[pos - 1] = active_clients[pos];
    }
    nclients_activate--;
    printf("<<delete user from logins\n");
}

void handle_sigint()
{
    close(fd_server);
    printf("CLIENTS SERVER IS CLOSING\n");
    exit(0);
}

void recvfrom_nonblocking(int fd)
{
    // Retirado de: https://stackoverflow.com/questions/15941005/making-recvfrom-function-non-blocking
    struct timeval read_timeout;
    read_timeout.tv_sec = 45;
    read_timeout.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
}

void print_login_clients(){
    printf("REGISTER CLIENTS\n");
    if(nclients_activate == 0){
        printf("NO clients yet\n");
        return;
    }
    for(int i = 0; i<nclients_activate; i++)
        printf("Client %d\t %s:%s:%s\n", i, active_clients[i].user_id, active_clients[i].ip, active_clients[i].password);
}