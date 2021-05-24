void error(char *msg)
{
    printf("Error: %s\n", msg);
    exit(-1);
}

void input_menu()
{
    printf("Choose a type of communication [1 - 3]:\n"
    "1 - Client/Server\n"
    "2 - P2P\n"
    "3 - Group\n"
    "4 - Exit\n");
}

void client_server_comm()
{
    printf("Type the <user_id> you want to send a message to:\n");
}

void group_comm_create()
{
    printf("Choose the option for group communication [1 - 2]:\n"
    "1 - Create group\n"
    "2 - Join group\n");
}
