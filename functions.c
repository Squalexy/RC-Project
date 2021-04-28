char *concat(const char *string1, const char *string2)
{
    char *concat_string = malloc(strlen(string1) + strlen(string2) + 1);
    strcpy(concat_string, string1);
    strcat(concat_string, string2);
    return concat_string;
}

void error(char *msg)
{
    printf("Error: %s\n", msg);
    exit(-1);
}