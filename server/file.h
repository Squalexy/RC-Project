#ifndef FILE_HEADER
#define FILE_HEADER
#include <stdio.h>
#include <semaphore.h>
#include "global.h"
#define MAX_LINE 200

#define INVALID "---------"

sem_t mutex_registers;
//user_t convert_user(char *string);
int is_null_user(user_t user);
long count_clients(FILE *registers);
void list_file();
int delete_from_file(char *username);
int add_to_file(user_t user);
void init_mutex_registers();
user_t convert_string_to_user(char *string);
void close_mutex_registers();
void convert_user_struct_in_string(user_t user, char *user_string);
int search_user(char * user_id, user_t * user);
#endif