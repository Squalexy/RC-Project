#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFFER_LEN 512
#define PORT 9876

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}

int main(void)
{
	int fd;
	char buffer[BUFFFER_LEN];
	char message[BUFFFER_LEN];

	struct sockaddr_in addr_server, addr_client;

	socklen_t client_len = sizeof(addr_client);

	int received_len;

	if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		erro("Socket");

	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(PORT);
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(fd, (struct sockaddr *)&addr_server, sizeof(addr_server)) == -1)
		erro("criacao do Bind");

	if ((received_len = recvfrom(fd, buffer, BUFFFER_LEN, 0, (struct sockaddr *)&addr_client, (socklen_t *)&client_len)) == -1)
		erro("Recvfrom");

	buffer[received_len] = '\0';
	printf("Recebi uma mensagem do sistema com o endereço %s e o porto %d\n", inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port));
	printf("Conteúdo da mensagem: %s\n", buffer);

	snprintf(message, BUFFFER_LEN, "N.o carateres = %d", received_len);

	sendto(fd, (const char *)message, strlen(message), 0, (struct sockaddr *)&addr_client, sizeof(addr_client));

	return 0;
}