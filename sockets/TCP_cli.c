// Cliente TCP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXBUFSIZE 1000

int main()
{
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   char sendline[MAXBUFSIZE];
   char recvline[MAXBUFSIZE];

   // Abre um socket TCP
   sockfd=socket(AF_INET,SOCK_STREAM,0);

   // Define IP e porta de destino
   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
   
   servaddr.sin_port=htons(32000); //Big-endian x little endian

   system("clear");
   printf("\t##### TCP client #####\n");
      
   // Escreve dados no buffer
   strcpy(sendline, "Hello!");

   // Inicia conexao com o servidor
   connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

   // Envia
   printf("\tSending hello to TCP server 127.0.0.1");

   // Envia dados
   sendto(sockfd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));

   printf("\tWaiting for response...\n");

   // Recebe resposta
   recvline[0] = '\0';
   n = recvfrom(sockfd, recvline, MAXBUFSIZE, 0, NULL, NULL);
   recvline[n]='\0';
   
   if (recvline[0] != '\0'){
		printf("\tReceived data:\n\t%s\n\n", recvline);
	}
}
