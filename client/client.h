#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// client functions
void execute_client(int argc, char **argv, char *endServer, struct sockaddr_in addr_server, int fd);
int authentication(int fd, struct sockaddr_in addr_server, char *username, char *password);
void communication(int server_fd, struct sockaddr_in addr_server, char *username);
void recvfrom_nonblocking(int fd);
int p2p_communication(char *user_destination_ip, char *user_destination_port);
void error(char *msg);
int is_error(char *string);
void input_menu();
int get_one_line(FILE * fich, char *linha, int lim);
void *chat();

#endif