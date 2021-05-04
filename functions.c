void error(char *msg)
{
    printf("Error: %s\n", msg);
    exit(-1);
}

void input_menu()
{
    printf("Choose a type of communication [1 - 3]:\n1 - Client/Server\n2 - P2P\n3 - Group\n4 - Exit");
}