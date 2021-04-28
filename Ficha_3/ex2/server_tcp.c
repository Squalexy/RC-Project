/*******************************************************************************
 * Ficha 3 Exercicio 2 PL7
 * Alexy Almeida Nº2019192123
 * Joana Simoes Nº2019217013
 *******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>

#define SERVER_PORT     9000
#define VALID "VALID"
#define TRUE 1
#define FALSE 0
#define TOTAL_VALUES 10
#define BUF_SIZE 1024

void process_client(int fd, struct sockaddr_in client_info);
void erro(char *msg);
void send_to(int client_fd, char * message);
void make_clean(int *values);
int sum( int * values);
float mean (int * values);
int is_number(char * string);

int fd, client;

void sigint(){
  close(client);
  printf("\nTerminating process %ld \n", (long)getpid());
  close(fd);
  exit(0);
}
int main() {
  struct sockaddr_in addr, client_addr;
  int client_addr_size;

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(SERVER_PORT);

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	erro("na funcao socket");
  if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
	erro("na funcao bind");
  if( listen(fd, 5) < 0)
	erro("na funcao listen");
  client_addr_size = sizeof(client_addr);

  signal(SIGINT, sigint);

  while (1) {
    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while(waitpid(-1,NULL,WNOHANG)>0);
    //wait for new connection
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
    if (client > 0) {
      if (fork() == 0) {
        process_client(client, (struct sockaddr_in)client_addr);
        close(client);
        exit(0);
      }
    }
  }
  close(fd);
  return 0;
}

void process_client(int client_fd, struct sockaddr_in client_info)
{

  char ip_address[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_info.sin_addr,ip_address, INET_ADDRSTRLEN-1);

  printf("CONNECTION FROM  (IP: port) %s:%d\n", ip_address, client_info.sin_port);

	int nread = 0;
	char buffer[BUF_SIZE];
  int num_values = 0;
  int * values = (int*)malloc(sizeof(int)*TOTAL_VALUES);
  make_clean(values);

  int go = TRUE;
  do{
    nread = read(client_fd, buffer, BUF_SIZE-1);
    buffer[nread] = '\0';
    if(!strcmp(buffer, "SAIR"))
      go = FALSE;
    else if(!strcmp(buffer, "MEDIA")){
      if(num_values == TOTAL_VALUES){
        float val = mean(values);
        char mean_values[100];
        snprintf(mean_values, 100, "MEDIA: %.2f", val);
        send_to(client_fd, mean_values);
      }else{
        send_to(client_fd, "ERRO: sem valores");
      }

    }else if(!strcmp(buffer, "SOMA")){

      if(num_values == TOTAL_VALUES){
        int val = sum(values);
        char sum_values[100];
        snprintf(sum_values, 100, "SOMA: %d", val);
        send_to(client_fd, sum_values);
      }
      else{
        send_to(client_fd, "ERRO: sem valores");
      }

    }else if(!strcmp(buffer, "DADOS")){
      num_values = 0;
      char number[BUF_SIZE];
      for(int i=0; i< TOTAL_VALUES; i++){
        nread = read(client_fd, number, BUF_SIZE-1);
        if(is_number(number)){
          send_to(client_fd, VALID);
          *(values+i)=atoi(number);
          num_values++;
        }
        else{
          send_to(client_fd, "ERRO: numero invalido");
          make_clean(values);
          num_values = 0;
          break;
        } 
      }

    }else
      send_to(client_fd, "Erro: comando nao existente");
    
  } while (go);

  free(values);
  fflush(stdout);
	close(client_fd);
  printf("DISCONNECTED FROM  (IP: port) %s:%d\n", ip_address, client_info.sin_port);

}
void send_to(int client_fd, char * message){
  write(client_fd, message, 1 + strlen(message));
}
void erro(char *msg){
	printf("Erro: %s\n", msg);
	exit(-1);
}

void make_clean(int * values){
  for(int * aux = values; aux < values + TOTAL_VALUES; aux++)
    *aux = 0;
}

int sum( int *values){
  int val=0;
  for(int * aux = values; aux < values + TOTAL_VALUES; aux++)
    val += *aux;
  
  return val;
}
float mean( int *values){
  int val = sum(values);
  
  return (float)val/TOTAL_VALUES;
}

int is_number(char * string) {
    if(string == NULL)
        return FALSE;
    for(char * p = string; *p != '\0'; p++){
        if(!isdigit(*p))
            return FALSE;
    }
    return TRUE;
}


