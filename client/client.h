#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// client functions
void execute_client(int argc, char *argv, char *endServer, struct hostent *hostPtr, struct sockaddr_in addr_server, int fd);
void authentication(int fd, struct sockaddr_in addr_server, char *username, char *password);
void communication(int server_fd, struct sockaddr_in addr_server, char *username);
void recvfrom_nonblocking(int fd);
int received_from_server(int server_fd);
int p2p_communication(char *user_destination_ip, char *user_destination_port);
void error(char *msg);
int is_error(char *string);
void input_menu();

#endif FUNCTIONS_H