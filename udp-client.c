// Cliente UDP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXBUFSIZE 1000 

void handleTemperatureReading(char *buffer) {
 char temp[10];
   printf("buffer %c%c%c", buffer[1], buffer[2], buffer[3]); 
   // for (int i = 1 ; i < strlen(buffer) - 1; i++) { 
   sprintf(temp, "%c%c", buffer + 1, buffer + 2); 
   printf("depois do temp %c\n", temp[0]);   

  // char str1[] = "19785551212";
  // char str2[40];
  // sprintf(str2, "1-%.3s-%.3s-%.4s", str1 + 1,  str1 + 4, str1 + 7);
  // Serial.println(str2);
   
  // strcat(temp, buffer[i]);
  // }
  printf("oi %s", temp);
}

/*
O tamanho max. do datagrama UDP é 65535 bytes, que é o valor máximo 
representado pelos dois bytes destinados ao tamanho do datagrama UDP.		
Descontados os cabeçalhos do UDP e do IP temos 65507 bytes.
*/

int main()
{
	int sock, status, socklen;
	unsigned char buffer[MAXBUFSIZE];
	struct sockaddr_in saddr;

	// Preenche com zeros a struct sockaddr_in
	memset(&saddr, 0, sizeof(struct sockaddr_in));

	/*
	Abre um socket UDP
	PFINET: domínio Internet
	SOCK_DGRAM: tipo Datagrama
	IPPROTO_IP: protocolo da camada de rede
	*/
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	if ( sock < 0 )
	{
		printf("Erro socket\n");
		exit(0);
	}

	// Define família (Internet), IP (para a mesma máquina - localhost) e porta de destino (36000)
	saddr.sin_family = PF_INET;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = htons(36000); 
	/* 
	htons: conversão do tipo host para rede (ordem dos bytes - big endian x little endian)
	36000 é uma porta não registrada junto a IANA
	*/

	// O IP de origem é 127.0.0.1 e a porta será atribuída automaticamente (porta alta)

 	system("clear");
	printf("\t\t\t Cliente UDP\n");
	printf("\t-> Enviando Oi para o servidor (IP 127.0.0.1 e porta 36000)\n");

	// Escreve dados no buffer
	strcpy(buffer, "rt");
  
	//Obtem tamanho em bytes da struct sockaddr_in
	socklen = sizeof(struct sockaddr_in); 
   
	// Envia mensagem
	status = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&saddr, socklen);
	/*
	Descritor: sock
	Onde estão os dados a serem enviados: buffer
	Número de bytes a serem enviados:  strlen(buffer) (tamanho da string)
	Flags: 0 
	Endereço de destino: &saddr
	Tamanho da struct de endereço: socklen
	*/

	// Preenche o buffer com zeros para usá-lo para receber resposta
	memset(&buffer, 0, sizeof(buffer));
      
	printf("\t-> Aguardando resposta...\n");
   
	// Recebe resposta
	status = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&saddr, &socklen);
	/* 
	Descritor: sock
	Para onde irão os dados recebidos: buffer
	Número máximo de bytes a serem recebidos: MAXBUFSIZE (1000)
	Flags: 0 
	Endereço da origem: &saddr
	Tamanho da struct de endereço: socklen
	*/

	printf("\t-> Resposta recebida do servidor com IP %s e Porta %d\n\t%s\n", inet_ntoa(saddr.sin_addr), htons(saddr.sin_port), buffer);
	// inet_ntoa: converte IP para a notação a.b.c.d

  if (buffer[0] == 't') {
    handleTemperatureReading(buffer);
  }
      
	// Desabilita e fecha o socket
	shutdown(sock, SHUT_RDWR );
	//close(sock);

	return 0;
}