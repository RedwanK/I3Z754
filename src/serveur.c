/* Fichier: serveur.c
 * Communication client-serveur
 * Auteurs: John Samuel, Redwan Kara, Emilio Maldonado
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "serveur.h"
#include "jsonParser.h"
#include "validateur.h"

void plot(char *data) {
  //Extraire le compteur et les couleurs RGB
  FILE *p = popen("gnuplot -persist", "w");
  printf("Plot \n");
  int count = 0;
  float n;
  float angle;
  char *saveptr = NULL;
  char *str = data;
  char *strNbColors;
  char* tiktok;
  tiktok = strtok(data, " ");
  fprintf(p, "set xrange [-15:15]\n");
  fprintf(p, "set yrange [-15:15]\n");
  fprintf(p, "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "set title 'Top 10 colors'\n");
  fprintf(p, "plot '-' with circles lc rgbcolor variable\n");
  while(1)
  {
    if (count == 0)
    {
      n = atof(tiktok);
      angle = (360/n);
    }
    else
    {
      // Le numéro 36, parceque 360° (cercle) / 10 couleurs = 36
      fprintf(p, "0 0 10 %f %f 0x%s\n", (count-1)*angle, count*angle, tiktok+1);
    }
    tiktok = strtok(NULL, " ");
    if (tiktok == NULL)
    {
      break;
    }
    count++;
  }
  fprintf(p, "e\n");
  printf("Plot: FIN\n");
  pclose(p);
}

/* resend a message (*data) to user (cint recois_envoie_message(int socketfd)lient_socket_fd)
 */
int renvoie(int client_socket_fd, char *data, char *code)
{
  char answer[2000] = "";
  strcat(answer, code);
  strcat(answer,": ");
  strcat(answer, data);
  transformToJson(answer);

  if(valideJson(answer, 0) != 0){
      perror("ERREUR ! Syntaxe Json incorrecte.");
      return -1;
  }
    printf("ANSWER : %s", answer);
  int data_size = write (client_socket_fd, (void *) answer, strlen(answer));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

//receive a message and ask the server to answer this message
int recois_message(int client_socket_fd, char* data, char *code)
{
  char message[1000], returnData[1000] = "";
  memset(returnData, 0, sizeof(returnData));
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, 1024, stdin);
  message[strlen(message)-1] = '\0';
  strcat(returnData, message);

  renvoie(client_socket_fd, returnData, code);
}

/* decode and execute mathematical operation asked by the user */
int recois_numero_calcul(int client_socket_fd, char* data, char* code)
{
 char operateur[10], numStr1[10], numStr2[10];
 char result[200] = "Veuillez entrer un opérateur arithmétique valable (+|-|*|/) [division par 0 interdite !]";

 //cut the string
 sscanf(data, "%s %s %s", operateur, numStr1, numStr2);

 float numFloat1 = atof(numStr1);
 float numFloat2 = atof(numStr2);
 float resultat;

 //switch for different operations
 if(strcmp(operateur, "+") == 0)
 {
	resultat = numFloat1 + numFloat2;
	gcvt(resultat, 3, result);
 } else if(strcmp(operateur, "-") == 0)
 {
	resultat = numFloat1 - numFloat2;
	gcvt(resultat, 3, result);
 } else if (strcmp(operateur, "*") == 0)
 {
	resultat = numFloat1 * numFloat2;
	gcvt(resultat, 3, result);
 } else if ((strcmp(operateur, "/") == 0) && numFloat2 != 0)
 {
	resultat = numFloat1/numFloat2;
	gcvt(resultat, 3, result);
 } else
 {
	printf("Mauvais calcul\n");
 }

 //return result to client
 renvoie(client_socket_fd, result, code);

}

/* decode and save colors in a txt file */
int recois_couleurs(int client_socket_fd, char* data, char *code)
{
 FILE * fd;
 char operateur[10], numStr1[10], numStr2[10];
 int nbCouleurs;
 //cut the string

 sscanf(data, "%d,", &nbCouleurs);
 if(nbCouleurs > 30)
 {
   char result[200] = "Le nombre de couleurs ne doit pas exceder 30.";
   renvoie(client_socket_fd, result, code);
   return 0;
 }
 char* tiktok = strtok(data, " ");
 int i = 1;
 //open the txt file
 fd = fopen("colors.txt", "a+");
 for(i; i <= nbCouleurs; i++)
 {
    tiktok = strtok(NULL, " ");
   	if(tiktok == NULL)
	{
	  char result[200] = "Le nombre de couleurs ne correspond pas aux entrées, les couleurs ont quand même été enregistrées.";
	  fclose(fd);
	  renvoie(client_socket_fd, result, code);
	  return 0;
	}
   //put color at the end of the file
   fputs(tiktok, fd);
   fputs("\n", fd);
 }
 //close the file
 fclose(fd);
 //if number > real color number
 tiktok = strtok(NULL, " ");
 if(tiktok != NULL)
 {
   char result[200] = "Le nombre de couleurs est inférieur au nombre d'entrées, les couleurs excedantes n'ont pas été enregistrées";
   renvoie(client_socket_fd, result, code);
   return 0;
 }
 char result[100] = "Couleurs enregistrées\0";
 //return answer
 renvoie(client_socket_fd, result, code);
 return 0;
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd) {
  struct sockaddr_in client_addr;
  char data[1024], code[200];

  int client_addr_len = sizeof(client_addr);

  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *) &client_addr, &client_addr_len);
  if (client_socket_fd < 0 ) {
    perror("accept");
    return(EXIT_FAILURE);
  }

  memset(data, 0, sizeof(data));

  //lecture de données envoyées par un client
  int data_size = read (client_socket_fd, (void *) data, sizeof(data));

  if(valideJson(data, 1) != 0){
      perror("ERREUR ! Syntaxe Json incorrecte.");
      return -1;
  }
  if (data_size < 0) {
    perror("erreur lecture");
    return(EXIT_FAILURE);
  }

  decodeFromJson(data, code);
  printf ("Reception : %s\n", data);

  if (strcmp(code, "message") == 0) {
 	recois_message(client_socket_fd, data, code);
  } else if (strcmp(code, "nom") == 0) {
	renvoie(client_socket_fd, data, code);
  } else if (strcmp(code, "calcul") == 0) {
	recois_numero_calcul(client_socket_fd, data, code);
  } else if (strcmp(code, "couleurs") == 0) {
	recois_couleurs(client_socket_fd, data, code);
  } else if (strcmp(code, "plot") == 0) {
	plot(data);
  }

  //fermer le socket
  close(socketfd);
}

int main() {

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

  return 0;
}
