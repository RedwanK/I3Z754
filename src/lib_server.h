#ifndef __LIB_SERVER_H__
#define __LIB_SERVER_H__

#define PORT 8089

/* accept new client connection and read sended.
 * Then the server answer
 */
int recois_envoie_message(int socketfd);

/* resend a message (*data) to user (cint recois_envoie_message(int socketfd)lient_socket_fd)
 */
int renvoie(int client_socket_fd, char *data, char *code);

//receive a message and ask the server to answer this message
int recois_message(int client_socket_fd, char* data, char *code);

/* decode and execute mathematical operation asked by the user */
int recois_numero_calcul(int client_socket_fd, char* data, char *code);

/* decode and save colors in a txt file */
int recois_couleurs(int client_socket_fd, char* data, char *code);

int getNumbers(char *data, int *tab, char *operateur);

float findMini(int *tab, int size);

float findMaxi(int *tab, int size);

float findEcart(int *tab, int size);

float findMoy(int *tab, int size);

#endif


