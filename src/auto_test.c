/* Fichier: serveur.c
 * Communication client-serveur
 * Auteurs: Redwan Kara, Emilio Maldonado
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "auto_test.h"
#include "serveur.h"
#include "jsonParser.h"
#include "validateur.h"
#include "client.h"
#include "bmp.h"
#include "lib_client.h"
#include "lib_server.h"

#define NOM "nom: NOM_TEST\0"
#define CALCUL "calcul: + 10 20 30\0"

int server_main() {

  int socketfd;
  int bind_status;
  int client_addr_len;

  struct sockaddr_in server_addr, client_addr;

  /*
   * Creation d'un socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if ( socketfd < 0 ) {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  //détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  bind_status = bind(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if (bind_status < 0 ) {
    perror("bind");
    return(EXIT_FAILURE);
  }

  listen(socketfd, 10);
  
  recois_envoie_message(socketfd);
  
  close(socketfd);
  return 0;
}

int client_main(char *message, char *assertMessage) {
  int socketfd;
  int bind_status;

  struct sockaddr_in server_addr, client_addr;

  /*
   * Socket creation
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if ( socketfd < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  //server details (port and address)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //ask for the server to connect
  int connect_status = connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if ( connect_status < 0 ) {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }
  
  envoie_recois(socketfd, message, "Reponse test ");

  assertTrue(message, assertMessage);
  //close socket
  close(socketfd);
}


int main() {
  char name[500] = "nom: rachide";
  char calc[500] = "calcul: + 20 10 20";
  char couleurs [500] = "couleurs: 3, #ffffff, #ababab, #000000";
  int fork_pid = fork();
  int count = 0;
  if(fork_pid == 0 )
  {
  
    client_main(calc, "50 ");
    client_main(name, "rachide ");
    client_main(couleurs, "Couleurs enregistrées ");

  } else {
    server_main();
  }
  
  return 0;
}

int assertTrue(char* data, char* answer) 
{
  if (strcmp(data, answer) == 0 )
  {
    printf("TEST SUCCESS \n");
    return 0;
  }
  printf("TEST FAILED \n");
  return 1;
}