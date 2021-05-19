
#include "server_clients.h"
//para tratar dos clientes

void login_user(char *token);
void group_communication_acess(token);

int fd_server;
struct sockaddr_in addr_client;
void send_message(char * format, ...);
int find_register(char * ip, user_t * user);

void server_to_clients(char *port_clients) {

    /*************************UDP****************************/

    printf("Server for clients [%d]\n", getpid());
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


    while (TRUE) {
        if ((rclen = recvfrom(fd_server, buffer, MESSAGE_LEN - 1, 0, (struct sockaddr *)&addr_client, (socklen_t *)&client_len)) == -1)
            perror("Error in recvfrom");
        buffer[rclen] = '\0';

        token = strtok(buffer, DELIM);

        // !!!!!!!!!!!!!!!!!!!!!!!!!!! VER O FORMATO DOS PEDIDOS !!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (!strcmp(token, "LOGIN")) 
            login_user(token);
        else if(!strcmp(token, "REQUEST_P2P"))
            p2p_request(token);
        else if(!strcmp(token, "REQUEST_CS"))
            client_server_request(token);
        else if(!strcmp(token, "REQUEST_GROUP"))
            group_communication_acess(token);
        
    }
}

/**
 * @brief function to handle the user's login
 * 
 * @param token 
 */
void login_user(char *token) {
    printf("LOGIN\n");
    char user_id[20];
    char password[20];

    token = strtok(NULL, DELIM);
    strcpy(user_id, token);
    token = strtok(NULL, DELIM);
    strcpy(password, token);

    user_t * user = search_user(user);

    if (user != NULL && strcmp(user->password, password) == 0){ //se ele encontrou e a palavra pass e valida
        send_message("COMMUNICATIONS:\nClient-Server: %s; P2P: %s; Group: %s", user->client_server, user->p2p, user->group);
    }
    send_error("Invalid registration");
}


/**
 * @brief see if a user is authorized to make a certain type of communication
 * 
 * @param type_communication the type of communication (1 «=» client-server, 2 «=» P2P, 3 «=» group)
 * @param user 
 * @return int 1 if authorizeed, 0 otherwise
 */
int validate_communication(int type_communication, user_t user) {
    int return_value;
    switch(type_communication){
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
 * @brief function to handle the P2P request of the clients
 * 
 * @param token //TODO 
 */
void p2p_request(char * token) {
    printf("P2P REQUEST\n");
    char user_id[20];
    char * port = "1234"; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHANGE PORT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
    token = strtok(NULL, DELIM);
    if(token == NULL){
        send_error("Invalid input");
        return;
    }
    user_t * user = (user_t *) malloc(sizeof(user));
    // !!!!!!!!!!!!!! FIND USER !!!!!!!!!!!!!!!
    if(find_user(inet_ntoa(addr_client.sin_addr), user) == 0){
        send_error("Needs login before");
        free(user);
        return;
    }
    
    strcpy(user_id, token);
    user_t * user = search_user(user_id);

    if(user == NULL)
        send_error("User not found");
    else{
        send_message("%s;IP:%s;PORT:%s", user_id, user->ip, port);
    }
    free(user);
}

void client_server_request(char * token){
    
}


// ****************************** SEND MESSAGES ******************************
/**
 * @brief function to send messages using UDP with a certain format
 * 
 * @param format the format of the message to send
 * @param ... arguments of the message
 */
void send_message(char * format, ...) {
    char send[MESSAGE_LEN];
    va_list arg;
    va_start(arg, format);
    vsprintf(send, format, arg);
    va_end(arg);
    send_to(fd_server, (const char*) send, strlen(send), 0, (struct sockaddr * )&addr_client,sizeof(addr_client));
}

/**
 * @brief function to send error messages to the client
 * 
 * @param type type of error
 */
void send_error(char * type) {
    char send[MESSAGE_LEN] = "ERROR: ";
    strcat(send, type);
    send_to(fd_server, (const char*) send, strlen(send), 0, (struct sockaddr * )&addr_client,sizeof(addr_client));
}


// ****************************** REGISTRATIONS LIST ******************************

static node_t * root;
static user_t null_user = {"", "", "", "", "", ""};

int add_register(user_t user) {
    node_t * aux, *next, *prev;

    aux = (node_t *) malloc (sizeof(node_t));

    if(aux == NULL)
        return 0;

    aux->user = user;
    aux->next = NULL;

    if(root == NULL)
        root = aux;
    else {
        prev = root;
        next = root->next;

        while(next != NULL) {
            prev = next;
            next = next->next;
        }
        prev->next = aux;
    }
    return 1;
}

int delete_register(){
    node_t * next, *prev;
    user_t user;

    if(root == NULL) 
        return 0;
    
    prev = NULL;

    for(next = root; next->next != NULL; next = next->next)
        prev = next;
    
    if(prev == NULL){ //only one register in the list
        free(root);
        root = NULL;
    } else {
        prev->next = NULL;
        free(next);
    }
    return 1;
}

int find_register(char * ip,  user_t * user) {

    node_t * next, *prev;

    user_t aux_user;

    if(root == NULL){
        user = NULL;
        return 0;
    }

    int found = 0;
    for(next = root; next->next != NULL; next = next->next){
        if(strcmp(next->user.ip, ip) == 0){
            (*user) = next->user;
            return 1;
        }
    }
    user = NULL;
    return 0;
}
