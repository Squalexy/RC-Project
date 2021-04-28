/*******************************************************************************
 * Ficha 3 Exercicio 2 PL7
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
#include "header.h"

#define VALID "VALID"
#define TRUE 1
#define FALSE 0
#define TOTAL_VALUES 10
#define BUF_SIZE 1024

void erro(char *msg);
void communication(int fd);

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
  if ((hostPtr = gethostbyname(endServer)) == 0)
    	erro("Nao consegui obter endereço");

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[2]));

  if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	  erro("socket");
  if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
	  erro("Connect");

  communication(fd);
  printf("DISCONNECTED FROM THE SERVER\n");
  close(fd);
  exit(0);
}

void send_to_server(int fd, char *to_send);
void erro(char *msg);
int get_one_line(FILE * fich, char *linha, int lim);
int received_invalid_number(int server_fd);
int received_from_server(int server_fd);
void send_data_to_server(int server_fd);



void communication(int server_fd){
  printf("CONNECTED TO THE SERVER\n");
  int go = TRUE;
  char command[BUF_SIZE];
  int nread;

  do{
    printf("Command: \n" );
    get_one_line(stdin, command, BUF_SIZE);
    send_to_server(server_fd, command);

    if(!strcmp(command, "SAIR")){
      go = FALSE;
    }
    else if(!strcmp(command, "DADOS")){
      send_data_to_server(server_fd);
    }
    else{
      nread = received_from_server(server_fd);
    }
    
  }while(go && nread>0); //termina quando manda para o servidor a mensagem "SAIR" ou quando não recebe dados do servidor
}


void send_data_to_server(int server_fd){
  for(int i = 0; i< 10; i++){
    printf("Number: \n" );
    char num[10];
    get_one_line(stdin, num, 10-1);
    send_to_server(server_fd, num);
    if( received_invalid_number(server_fd)== TRUE)
      break;
  }
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


int get_one_line(FILE * fich, char *linha, int lim) {
    int c, i;
    i = 0;
    while (isspace(c = fgetc(fich)));
    if (c != EOF) {
        if (!iscntrl(c)) linha[i++] = c;
    } else
        return c;
    for (; i < lim - 1;) {
        c = fgetc(fich);
        if (c == EOF)
            return c;
        if (c == '\n')
            break;
        if (!iscntrl(c)) linha[i++] = c;
    }
    linha[i] = 0;
    while ((c != EOF) && (c != '\n'))
        c = fgetc(fich);
    return c;
}
