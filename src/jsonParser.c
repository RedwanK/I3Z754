/* Fichier: jsonParser.c
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
#include "jsonParser.h"


//decode a JSON, fill values and code
int decodeFromJson(char* jsonResponse, char* codePrev)
{
  int count = 0, countGood = 0;
  int pass = 0;
  char *saveptr = NULL, code[20], miss[10], valeurs[500]="";
  char *str = jsonResponse;
  sscanf(jsonResponse, "{\n \"code\" : \"%s\",\n", code);
  code[strlen(code)-2] = '\0';
  while(1)
  {
    char *token = strtok_r(str, "\"", &saveptr);

    if (token == NULL)
    {
      break;
    }
    if (count == 6 ) {
        pass = 1;
    }
    if(pass == 1 && count != 6) {
       if(countGood%2 == 0) {
         strcat(valeurs, token);
         strcat(valeurs, " ");
        }
       countGood += 1;
    }
    str=NULL;
    count += 1;
  }
  strcpy(jsonResponse, valeurs);
  strcpy(codePrev, code);
}


//transform a string like "code: value1, value2" into a JSON Like {Code: "code", Valeurs: ["value1", "value2"]}
int transformToJson(char* data)
{
  char code[20], *valeurs, jsonResponse[2500];
  char *saveptr = NULL;
  strtok_r(data, ":", &valeurs);
  sscanf(data, "%s", code);
  char *str = valeurs;
  strcpy(jsonResponse, "{\n \"code\" : \"");
  strcat(jsonResponse, code);
  strcat(jsonResponse, "\", \n \"valeurs\" :  [");
  while(1)
  {
    char *token = strtok_r(str, ", ", &saveptr);
    if (token == NULL)
    {
      jsonResponse[strlen(jsonResponse)-1] = ']';
      break;
    }
    str=NULL;
    strcat(jsonResponse, "\"");
    strcat(jsonResponse, token);
    strcat(jsonResponse, "\",");
  }

  strcat(jsonResponse, "\n} \n");
  strcpy(data, jsonResponse);
  return 0;
}
