#include "file.h"

void init_mutex_registers()
{
    sem_init(&mutex_registers, 1, 1);
}

void close_mutex_registers()
{
    sem_destroy(&mutex_registers);
}
user_t * convert_to_user_struct(char *string)
{
    printf(">>convert to struct\n");
    //!!!!!!!!!!!!!!!!!!!!!!!! FAZER VERIFICAÇÔES !!!!!!!!!!!!!!!!!!!!!!!!
    user_t *user = (user_t *)malloc(sizeof(user_t));
    printf("after allocation\n");
    char *token;
    if((token = strtok(NULL, DELIM))==NULL) 
        return NULL;
    printf("after first strtok");
    strcpy(user->user_id, token);
    if((token = strtok(NULL, DELIM))==NULL) 
        return NULL;
    strcpy(user->ip, token);
    if((token = strtok(NULL, DELIM))==NULL) 
        return NULL;
    strcpy(user->password, token);
    if((token = strtok(NULL, DELIM))==NULL) 
        return NULL;
    strcpy(user->client_server, token);
    if((token = strtok(NULL, DELIM))==NULL) 
        return NULL;
    strcpy((char *) user->p2p, token);
    if((token = strtok(NULL, DELIM))==NULL) 
        return NULL;
    strcpy((char *) user->group, token);
    printf("<<end convert\n");
    return user;
}

user_t * find_register(char *username)
{
    user_t *user = NULL;
    user_t * aux;
    FILE *registers;
    if((registers = fopen(CLIENTS_FILE, "rb")) == NULL){
        return NULL;
    }
    while(fread(aux, sizeof(user_t), 1, registers) == 1){
        if(strcmp(aux->user_id, username) == 0){
            user = aux;
            break;
        }
    }
    fclose(registers);
    return user;
}
/*
    devolve 1 se correu tudo bem, 0 o contrario
*/
int add_register(user_t * user)
{
    sem_wait(&mutex_registers);
    FILE * registers = fopen(CLIENTS_FILE, "ab");
    if(find_register(user->user_id) != NULL || registers == NULL){
        fclose(registers);
        sem_post(&mutex_registers);
        return 0;
    }

    fwrite(user, sizeof(user_t), 1, registers);
    sem_post(&mutex_registers);
    return 1;


}

/**
 *
 * @param username
 * @return 1 if successfully, 0 otherwise
 */
int delete_from_file(char *username) {
    sem_wait(&mutex_registers);
    if (find_register(username) == NULL)
        return 0;
    FILE *file1, *file2;
    file1 = fopen(CLIENTS_FILE, "rb");
    file2 = fopen(AUX_FILE, "wb");
    if(file1 == NULL){
        sem_post(&mutex_registers);
        return 0;
    }
    if(file2 == NULL){
        fclose(file1);
        sem_post(&mutex_registers);
        return 0;
    }
    user_t user;
    while(fread(&user, sizeof(user_t), 1, file1) == 1){
        if( strcmp(user.user_id, username) != 0){
            fwrite(&user, sizeof(user_t), 1, file2);
        }
    }
    fclose(file1);
    fclose(file2);
    remove(CLIENTS_FILE);
    rename(AUX_FILE, CLIENTS_FILE);
    sem_post(&mutex_registers);
    return 1;
}

// !!!!!!!!!!!!! VER ISTO !!!!!!!!!!!!!!!!!!!
long count_clients(FILE * registers){
    fseek(registers, 0, SEEK_END);
    long count = (long)(ftell(registers)/sizeof(user_t));
    fclose(registers);
    return count;
}



