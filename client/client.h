#ifndef CLIENTS_H
#define CLIENTS_H



// client functions
int authentication(int fd, struct sockaddr_in addr_server, char *username, char *password);
void communication(int server_fd, struct sockaddr_in addr_server, char *username);
void recvfrom_nonblocking(int fd);
int p2p_communication(char *user_destination_ip, char *user_destination_port);
void error(char *msg);
int is_error(char *string);
void input_menu();
int get_one_line(FILE * fich, char *linha, int lim);
void *chat();
void remove_end_line(char *string);
#endif