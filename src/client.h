#ifndef __CLIENT_H__
#define __CLIENT_H__

#define PORT 8089

/* send and receive messages */
int envoie_recois(int socketfd, char* data, char* type);

/* select command type */
int select_type_message(int socketfd);

/* just display a help menu */
int display_help(int socketfd);

//encode a string into a JSON format
int transformToJson(char* data);

//find colors in an image
int analyse(char *pathname, char *data, int nbColors);

//decode a string form a JSON
int decodeFromJson(char* jsonResponse, char* codePrev);

#endif
