#include "file.h"

#define LINESIZE 200

void init_mutex_registers()
{
    sem_init(&mutex_registers, 1, 1);
}

void close_mutex_registers()
{
    sem_destroy(&mutex_registers);
}

void add_register(user_t *user)
{
    sem_wait(&mutex_registers);
    FILE *registers = open(CLIENTS_FILE, "a");
    fprintf(registers, "%s||%s||%s||%s||%s||%s", user->user_id, user->ip, user->password, user->client_server, user->p2p, user->group);
    fclose(registers);
    sem_post(&mutex_registers);
}

user_t *find_register(char *username)
{
    sem_wait(&mutex_registers);
    user_t *user = NULL;
    FILE *registers = fopen(CLIENTS_FILE, "r");
    char line[250];
    char *str, *token;
    while (fgets(line, 250, registers) != NULL)
    {
        strcpy(str, line);
        if ((token = strtok(token, DELIM) != NULL))
        {
            if (!strcmp(token, username))
            {
                user = convert_to_user_struct(line);
                break;
            }
        }
    }
    fclose(registers);
    sem_post(&mutex_registers);
}

user_t *convert_to_user_struct(char *string)
{

    //!!!!!!!!!!!!!!!!!!!!!!!! FAZER VERIFICAÇÔES !!!!!!!!!!!!!!!!!!!!!!!!
    user_t *user = (user_t *)malloc(sizeof(user_t));
    char *userId;
    char *IP;
    char *password;
    char *client_server, p2p, group;
    char *token;
    token = strtok(string, DELIM);
    strcpy(user->user_id, token);
    token = strtok(NULL, DELIM);
    strcpy(user->ip, token);
    token = strtok(NULL, DELIM);
    strcpy(user->password, token);
    token = strtok(NULL, DELIM);
    strcpy(user->client_server, token);
    token = strtok(NULL, DELIM);
    strcpy(user->p2p, token);
    token = strtok(NULL, DELIM);
    strcpy(user->group, token);

    return user;
}

void delete_from_file(char *username)
{
    FILE *file1, *file2;
    file1 = fopen(CLIENTS_FILE, "r");
    file2 = fopen(AUX_FILE, "w");
    char line[300];
    char compare[300];
    while (fgets(line, 300, file1) != NULL)
    {
        strcpy(compare, line);
        char *token = strtok(compare, DELIM);
        strcpy(compare, token);
        if (strcmp(compare, username) != 0)
        {
            fputs(line, file2);
        }
    }
    fclose(file1);
    fclose(file2);
    remove(FILE1);
    rename(AUX_FILE, FILE1);
}
