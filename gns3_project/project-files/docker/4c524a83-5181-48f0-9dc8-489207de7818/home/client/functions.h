#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define ERROR "ERROR"
extern void remove_end_line(char *string);
extern int get_one_line(FILE *fich, char *linha, int lim);
extern int is_error(char *string);
#endif