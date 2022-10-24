/* 
	Servidor UDP multicast (funciona no Linux)
	Esse arquivo deve ser compilado e executado primeiro.
	
	Você pode rodar esse exemplo (serv-mcast.c) em uma ou mais máquinas em uma rede local.
	Se rodar em duas máquinas, ambas receberão a mensagem do cliente ao mesmo 
	tempo, pois o multicast permite o envio de mensagens para múltiplos destinatários 
	através de um fluxo único.
	
	Depois de executar serv-mcast.c em uma ou mais máquinas 
	o cliente multicast (client-mcast.c) deve ser rodado em uma máquina só (pode ser na mesma que está rodando o serv-mcast.c)
	
	O teste mais simples é abrir uma aba do terminal, rodar o servidor e em outra aba rodar o cliente).
*/

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
   int sock, status, socklen;
   unsigned char buffer[MAXBUFSIZE];
   struct sockaddr_in saddr;
   struct ip_mreq imreq;
      
   memset(&saddr, 0, sizeof(struct sockaddr_in));
   memset(&imreq, 0, sizeof(struct ip_mreq));

   // Abre socket UDP
   sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
   if ( sock < 0 )
   {
     printf("Erro socket.\n");
     exit(0);
   }
   
   saddr.sin_family = PF_INET;
   saddr.sin_port = htons(36000); // Escuta na porta 36000
   saddr.sin_addr.s_addr = htonl(INADDR_ANY); // Recebe dados através de qualquer interface de rede
   status = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)); // Associa socket com IP e porta

   if ( status < 0 )
   {
     printf("Erro bind.\n");
     exit(0);
   }
   
	/*
		End. do grupo multicast, ou seja, os servidores que devem receber as mensagens desse grupo 
		usam esse mesmo IPPROTO_IP. 
		O IP unicast (ou seja, individual de cada máquina) é preservado
	*/	
	
   imreq.imr_multiaddr.s_addr = inet_addr("239.255.32.33"); 
   
   // Recebe dados através de qualquer interface de rede
   imreq.imr_interface.s_addr = INADDR_ANY; 

   // Se associa ao grupo multicast
   status = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
              (const void *)&imreq, sizeof(struct ip_mreq));

   socklen = sizeof(struct sockaddr_in);

   system("clear");
   printf("\t##### Servidor Multicast #####\n");
     
   while (1)
   {
   	// Recebe
   	status = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&saddr, &socklen);

    	buffer[status] = '\0';

    	printf("\tDados recebidos: %s\n", buffer);
  
  	printf("\tEnviando resposta...\n");
   	// Envia
   	strcpy(buffer, "Oi, eu sou o servidor!!!.");
    
   	status = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&saddr, socklen);
     
   }
}