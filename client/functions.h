#ifndef FUNCTIONS_H
#define FUNCTIONS_H
/*************** PROJETO DE REDES DE COMUNICACAO ***************/
// Joana Simoes, No 2019217013
// Alexy de Almeida No 2019192123

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define ERROR "ERROR"
extern void remove_end_line(char *string);
extern int get_one_line(FILE *fich, char *linha, int lim);
extern int is_error(char *string);
#endif