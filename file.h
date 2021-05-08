#ifndef FILE_HEADER
#define FILE_HEADER
#include <stdio.h>
#include <semaphore.h>
#include "global.h"

#define DELIM "||"
sem_t mutex_registers;

extern void delete_from_file(char *username);
extern void add_register(user_t *user);
extern user_t *convert_to_user_struct(char *string);

#endif