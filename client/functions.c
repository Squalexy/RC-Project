/*************** PROJETO DE REDES DE COMUNICACAO ***************/
// Joana Simoes, No 2019217013
// Alexy de Almeida No 2019192123
#include "functions.h"



void remove_end_line(char *string)
{
    while (*string && *string != '\n' && *string != '\r')
        string++;

    *string = '\0';
}

int is_error(char *string)
{
    int len_error = strlen(ERROR);

    if (len_error > strlen(string))
        return 0;
    char error[strlen(ERROR) + 1];
    strcpy(error, ERROR);
    for (unsigned int i = 0; i < len_error; i++)
    {
        if (error[i] != string[i])
            return 0;
    }
    return 1;
}

int get_one_line(FILE *fich, char *linha, int lim)
{
    int c, i;
    i = 0;
    while (isspace(c = fgetc(fich)))
        ;
    if (c != EOF)
    {
        if (!iscntrl(c))
            linha[i++] = c;
    }
    else
        return c;
    for (; i < lim - 1;)
    {
        c = fgetc(fich);
        if (c == EOF)
            return c;
        if (c == '\n')
            break;
        if (!iscntrl(c))
            linha[i++] = c;
    }
    linha[i] = 0;
    while ((c != EOF) && (c != '\n'))
        c = fgetc(fich);
    return c;
}
