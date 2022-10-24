// Servidor UDP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAXBUFSIZE 1000
#define MAX_TEMP 45
#define MIN_TEMP -45

void handleReadCommands(char *buffer) {
  printf("\tHandling Read Command...\n");
  if (buffer[1] == 't') {
    printf("\tReading Temperature...\n");
    // Preeenche o buffer com zeros
    memset(buffer, 0, sizeof(buffer));

    sprintf(buffer, "%s%03d", "t", getTemperatureReading());
  }
}

int getTemperatureReading() { 
  return MIN_TEMP + rand() / (RAND_MAX / (MAX_TEMP - MIN_TEMP + 1) + 1);
}

void handleLEDCommands(char *buffer) {
  printf("\tHandling LED Command...\n");
  if (buffer[1] == 'g') {
    printf("\tTurning red LED off\n");
    printf("\tTurning green LED on\n");
  }
  if (buffer[1] == 'r') {
    printf("\tTurning green LED off\n");
    printf("\tTurning red LED on\n");
  }
}

/*
O tamanho max. do datagrama UDP é 65535 bytes, que é o valor máximo
representado pelos dois bytes destinados ao tamanho do datagrama UDP.
Descontados os cabeçalhos do UDP e do IP temos 65507 bytes.
*/

int main() {
  int sock, status, socklen;
  unsigned char buffer[MAXBUFSIZE];
  struct sockaddr_in saddr;

  srand(time(NULL));

  // Preenche com zeros a struct sockaddr_in
  memset(&saddr, 0, sizeof(struct sockaddr_in));

  /*
  Abre um socket UDP
  PFINET: domínio Internet
  SOCK_DGRAM: tipo Datagrama
  IPPROTO_IP: protocolo da camada de rede
  */
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock < 0) {
    printf("Erro socket\n");
    exit(0);
  }

  /*
  Define família (Internet)
  Aguarda dados na porta 36000, que não é registrada junto a IANA
  htons: conversão do tipo host para rede (ordem dos bytes - big endian x little
  endian)
  */
  saddr.sin_family = PF_INET;
  saddr.sin_port = htons(36000);

  /*
  Bind: atribui o endereço especificado por &saddr para o socket
  INADDR_ANY: todas as interfaces locais
  htonl: conversão do tipo host para rede (ordem dos bytes - big endian x little
  endian)
  */
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  status = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

  if (status < 0) {
    printf("Erro bind\n");
    exit(0);
  }

  // Obtem tamanho em bytes da struct sockaddr_in
  socklen = sizeof(struct sockaddr_in);

  system("clear");
  printf("\t\t\t Servidor UDP\n");
  printf("\tAguardando...\n");

  while (1) {
    // Preeenche o buffer com zeros
    memset(&buffer, 0, sizeof(buffer));

    // Recebe dados do socket
    status = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&saddr,
                      &socklen);
    /*
    Descritor: sock
    Para onde irão os dados recebidos: buffer
    Número máximo de bytes a serem recebidos: MAXBUFSIZE (1000)
    Flags: 0
    Endereço da origem: &saddr
    Tamanho da struct de endereço: socklen
    */

    printf("\t-> Dados recebidos do cliente com IP: %s e Porta: %d\n\t%s\n",
           inet_ntoa(saddr.sin_addr), htons(saddr.sin_port), buffer);

    if (buffer[0] == 'r') {
      handleReadCommands(buffer);
      
      printf("\tEnviando resposta...\n");
  
      /*
      Descritor: sock
      Onde estão os dados a serem enviados: buffer
      Número de bytes a serem enviados:  strlen(buffer) (tamanho da string)
      Flags: 0
      Endereço de destino: &saddr
      Tamanho da struct de endereço: socklen
      */
      status = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&saddr,
                      socklen);
    }

    if (buffer[0] == 'l') {
      handleLEDCommands(buffer);
    }

  }

  return 0;
}