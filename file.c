#include "file.h"
user_t convert_string_to_user(char * string);
user_t return_invalid();


void init_mutex_registers()
{
    sem_init(&mutex_registers, 1, 1);
}

void close_mutex_registers() {
    sem_destroy(&mutex_registers);
}

long count_clients(FILE * registers){
    printf(">>count clients\n");
    fseek(registers, 0, SEEK_END);
    long count = (long)(ftell(registers)/sizeof(user_t));
    fclose(registers);
    printf("<<count client\n");
    return count;
}

int is_null_user(user_t user){
    printf(">>is nll user\n");
    if(strcmp(user.user_id, INVALID) == 0){
        printf("null user\n");
        return 1;
    }
    printf("not null user\n");
    return 0;
}
/**
 * method to print the user
 * @param user the user to print
 */
void print_user(user_t user){
    printf("User:::\n");
    printf("User: %s\nIP: %s\nPassWord: %s\nclient-server:%s\nP2P: %s\nGroup: %s\n",
           user.user_id,user.ip, user.password, user.client_server, user.p2p, user.group);
}

/**
 * funtion to list all the users in the file
 */
void list_file(){
    sem_wait(&mutex_registers);
    FILE * file = fopen(CLIENTS_FILE, "rb");
    user_t user;
    while(fread(&user, sizeof(user_t), 1, file)){
        printf("------------------------------------\n");
        print_user(user);
        printf("------------------------------------\n");
    }
    fclose(file);
    sem_post(&mutex_registers);
}
/**
 * function to delete a certain user from a file
 * @param username the user_id to delete
 * @return 1 if all goes well or 0 otherwise
 */
int  delete_from_file(char * username){
    printf(">>delete_from_file\n");
    sem_wait(&mutex_registers);
    FILE * original = fopen(CLIENTS_FILE, "rb");
    FILE * final = fopen("aux_file.bin", "wb");
    if(original == NULL){
        printf("Error opening the original file\n");
        sem_post(&mutex_registers);
        return 0;
    }
    if(final == NULL){
        printf("Error opening the final file\n");
        sem_post(&mutex_registers);
        return 0;
    }
    int was_found = 0;
    user_t user;
    while(fread(&user, sizeof(user_t), 1, original)){
        if(strcmp(username, user.user_id)!=0){
            fwrite(&user, sizeof(user_t), 1, final);
        }else
            was_found = 1;
    }
    fclose(original);
    fclose(final);
    remove(CLIENTS_FILE);
    rename("aux_file.bin", CLIENTS_FILE);
    sem_post(&mutex_registers);
    printf("<<delete from file\n");
    return was_found;
}
/**
 * function to find a certain client in the file
 * @param username the name to search
 * @return a pointer to the user or NULL
 */
user_t * find_in_file(char * username){
    printf(">>find in file\n");
    FILE * file = fopen(CLIENTS_FILE, "rb");
    if(file == NULL)
        return NULL;
    user_t * user = (user_t *) malloc(sizeof(user_t));

    while(/*!feof(file) &&*/ fread(user, sizeof(user_t), 1, file)){
        printf("searching...\n");
        if(strcmp(user->user_id, username) == 0) {
            printf("%s was found\n", username);
            fclose(file);
            return user;
        }
    }
    fclose(file);
    free(user);
    return NULL;
}

/**
 * function to add a new client to the file
 * @param user the user to add
 * @return 1 if all goes well or 0 otherwise
 */
int add_to_file(user_t user){
    printf(">>add_to_file\n");
    sem_wait(&mutex_registers);
    user_t * found = find_in_file(user.user_id);
    if(found != NULL){
        free(found);
        sem_post(&mutex_registers);
        return 0;
    }
    FILE * file = fopen(CLIENTS_FILE, "ab");
    fwrite(&user, sizeof(user_t), 1, file);
    printf("User %s was added\n", user.user_id);
    fclose(file);
    sem_post(&mutex_registers);
    return 1;
}

int invalid_config(char * string){
    if(strcmp(string, "no") == 0 || strcmp(string, "yes") == 0)
        return 0;
    else
        return 1;
}
user_t return_invalid(){
    user_t user;
    strcpy(user.user_id, INVALID);
    strcpy(user.ip, INVALID);
    strcpy(user.password, INVALID);
    return user;
}
int invalid_ip(int ip_part){
    if(ip_part>=0 && ip_part<=255)
        return 0;
    return 1;
}

user_t convert_string_to_user(char * string){
    printf(">>convert string to user\n");
    char user_id[20];
    char ip [37];
    char password[20];
    char client_server[10];
    char p2p[10];
    char group[10];
    if(sscanf(string, "ADD;%[a-zA-Z0-9];%[0-9.];%[a-zA-Z0-9 +];%[yesno];%[yesno];%[yesno]", user_id, ip, password, client_server, p2p, group) !=6){
        printf("invalid number of arguments\n");return return_invalid();
    }

    int p1, p2, p3, p4;

    if(sscanf(ip, "%d.%d.%d.%d", &p1, &p2, &p3, &p4) != 4 || invalid_ip(p1) || invalid_ip(p2) || invalid_ip(p3) || invalid_ip(p4)){
        printf("Invalid ip");
        return return_invalid();
    }

    if(invalid_config(client_server) || invalid_config(p2p) || invalid_config(group)){
        printf("Invalid config\n");
        return return_invalid();
    }


    user_t user;
    strcpy(user.user_id, user_id);
    strcpy(user.ip, ip);
    strcpy(user.password, password);
    strcpy(user.client_server, client_server);
    strcpy(user.p2p, p2p);
    strcpy(user.group, group);
    return user;
}

void convert_user_struct_in_string(user_t user, char * user_string){

    snprintf(user_string, MESSAGE_LEN-1, "%s;%s;%s;%s;%s;%s", user.user_id, user.ip, user.password, user.client_server, user.p2p, user.group);

}



