/*******************************************************************************
 * Alexy Almeida Nº2019192123
 * Joana Simoes Nº2019217013
 * 
 * USO: >cliente <enderecoServidor>  <porto>
 * 
 * Porto a usar: 9000
 *******************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include "global.h"
#define VALID "VALID"
#define TRUE 1
#define FALSE 0
#define TOTAL_VALUES 10
#define BUF_SIZE 1024

void erro(char *msg);
void communicate(int fd);
void remove_end_line(char *string);
int get_one_line(FILE *fich, char *linha, int lim);
int is_error(char *string);
int authentication(int fd, struct sockaddr_in addr_server);


int main(int argc, char *argv[]) {
  char endServer[100];
  int fd;
  struct sockaddr_in addr;
  struct hostent *hostPtr;

  if (argc != 3) {
    	printf("cliente <host> <port>\n");
    	exit(-1);
  }

  strcpy(endServer, argv[1]);
  if (strcmp(argv[1], IP_SERVER))
  {
      error("endereço tem que ser 193.136.212.243 (interface externa de R3");
  }

  if ((hostPtr = gethostbyname(endServer)) == 0)
    	erro("Invalid IP address");

  if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	    erro("Socket");

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[2]));


  if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
	    erro("Connect");

  while (authentication(fd, addr) == 0);

  //* ------------------ COMUNICACAÇÃO ------------------ //

  communicate(fd);
  printf("\nDISCONNECTED FROM THE SERVER\n");
  close(fd);
  exit(0);
}

void send_to_server(int fd, char *to_send);
void erro(char *msg);
int get_one_line(FILE * fich, char *linha, int lim);
int received_from_server(int server_fd);
void receive_clients(int server_fd);

int authentication(int fd, struct sockaddr_in addr_server )
{
    char username[20], password[20];
    socklen_t slen = sizeof(addr_server);

    printf("Username: ");
    get_one_line(stdin, username, 20);
  
    printf("Password: ");
    get_one_line(stdin, password, 20);

    remove_end_line(username);
    remove_end_line(password);

    printf("--> Username: \"%s\"\n", username);
    printf("--> Password: \"%s\"\n", password);

    char id_info[MESSAGE_LEN];
    printf("Before snprintf...\n");
    snprintf(id_info, MESSAGE_LEN, "%s;%s;%s", LOGIN, username, password);
    
    printf("\nGOING TO SEND: %s\n", id_info);
    send_to_server(fd, id_info);

    // ------------------ RECEBER RESPOSTA ------------------ //

    char buffer[BUF_SIZE];
    int nread;
    do{
    nread = read(fd, buffer, BUF_SIZE-1);
    }while(nread<0);

    printf("\n--- MESSAGE RECEIVED ---\n");

    printf("%s", buffer);

    if (is_error(buffer))
    {
        erro("CLIENT_SERVER");
        return 0;
    }
    return 1;
}

void communicate(int server_fd){
  printf("\n*********************\nCONNECTED TO THE SERVER\n*********************\n\n");
  int go = TRUE;
  char command[BUF_SIZE];
  int nread;

  do{
    printf("Command: \n" );
    get_one_line(stdin, command, BUF_SIZE);
    remove_end_line(command);
    send_to_server(server_fd, command);

    if(!strcmp(command, "QUIT")){
      go = FALSE;
    }else if(!strcmp(command, "LIST")){
      receive_clients(server_fd);
    }
    else{
      nread = received_from_server(server_fd);
    }
    
  }while(go && nread>0); 
  printf("COMMUNICATION CLOSED\n");
}


void receive_clients(int server_fd){
  char message[BUF_SIZE];
  int can_go = 1;
    do{
      int n = read(server_fd, message, BUF_SIZE-1);
      if(n < 0){
        can_go = 0;
        send_to_server(server_fd, "ERROR");
      }
      message[n] = '\0';
      if(strcmp(message, "FINAL") == 0){
        can_go = 0;
      }
      else{
        printf("%s\n", message);
        send_to_server(server_fd, "VALID");
      }
    }while(can_go);
    printf("end list\n");
}


int received_invalid_number(int server_fd){
  char buffer[BUF_SIZE];
  int nread;
  do{
    nread = read(server_fd, buffer, BUF_SIZE-1);
  }while(nread<0);

  buffer[nread] = '\0';
  if(strcmp(buffer, VALID)==0){
    return FALSE;
  }
  printf("%s\n",buffer);
  return TRUE;
}


int received_from_server(int server_fd){
  char buffer[BUF_SIZE-1];
  int nread = read(server_fd, buffer, BUF_SIZE);
  buffer[nread] = '\0';
  printf("%s\n", buffer);
  return nread;
}


void send_to_server(int fd, char *to_send){
  write(fd, to_send, 1 + strlen(to_send));
}


void erro(char *msg) {
	printf("Erro: %s\n", msg);
	exit(-1);
}
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




