/* Fichier: validateur.c
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
#include "validateur.h"

int valideJson(char *message, int from)
{
    char json[2000] = "";
    strcpy(json, message);
    if(json[0] != '{')
    {
        perror("ERREUR DE SYNTAXE JSON");
        return -1;
    }

    //get the first line of the json (the one for the code)
    char code[20] = "", index[20] = "", valeurs[1000]="";
    sscanf(json, "{\n %s : %s,\n", index, code);
    index[strlen(index)] = '\0';
    code[strlen(code)-1] = '\0';
    //check if its correctly writen (code)
    if(valideCode(index, code) != 0)
    {
        perror("ERREUR DE SYNTAXE JSON POUR LE CODE.");
        return -1;
    }

    //check if the values corresponds to JSON syntax
    if(valideValeurs(json) != 0)
    {
        perror("ERREUR DE SYNTAXE JSON POUR LES VALEURS.");
        return -1;
    }

    dataConsistency(code, message, from);

    return 0;
}

int dataConsistency(char *code, char *valideJson, int from)
{
    char nJson[2000] = "";
    strcpy(nJson, valideJson);
    char *json = nJson;
    char index[500] = "", valeurs[1000]="", *cleanValues;
    char *saveptr = NULL;
    //here we take the first line of the json (which correspond to '{')
    char *token = strtok_r(json, "\n", &saveptr);
    json=NULL;
    //here we take the second line of the json (which correspond to '"code" : "message"')
    token = strtok_r(json, "\n", &saveptr);
    json=NULL;
    //here we take the third line of the json (which correspond to '"valeurs" : ["valeur1", "valeur2"]')
    token = strtok_r(json, "\n", &saveptr);
    json=NULL;
    //this line 'token value is what  we want to control
    sscanf(token, "%s : %s", index, valeurs);
    sscanf(code, "\"%s\"", code);
    code[strlen(code)-1] = '\0';

    if(strcmp(code, "message") == 0)
    {
        if(checkMessage(valeurs)!=0) return -1;
        return 0;
    } else if (strcmp(code, "nom") == 0)
    {
        if(checkName(valeurs)!=0) return -1;
        return 0;
    } else if (strcmp(code, "calcul") == 0)
    {
        if(checkCalcul(valeurs, from)!=0) return -1;
        return 0;
    } else if (strcmp(code, "couleurs") == 0)
    {
        if(checkCouleurs(valeurs, from)!=0) return -1;
        return 0;
    } else if (strcmp(code, "plot") == 0)
    {
        return 0;
    } else
    {
        perror("Opération non autorisée");
        return -1;
    }
}

int checkMessage(char *values)
{
    sscanf(values, "[%s]", values);
    values[strlen(values)-1] = '\0';
    char *saveptr = NULL;
    char *token = strtok_r(values, ",", &saveptr);
    while(token != NULL)
    {
        if(valideIndex(token, '"', '\0') != 0 )
        {
            return -1;
        }
        values = NULL;
        token = strtok_r(values, ",", &saveptr);
    }

    return 0;
}

int checkName(char *values)
{
    sscanf(values, "[%s]", values);
    values[strlen(values)-1] = '\0';
    char *saveptr = NULL;
    char *token = strtok_r(values, ",", &saveptr);
    while(token != NULL)
    {
        if(valideIndex(token, '"', '\0') != 0 )
        {
            return -1;
        }
        values = NULL;
        token = strtok_r(values, ",", &saveptr);
    }

    return 0;
}

int checkCalcul(char *values, int from)
{
    sscanf(values, "[%s]", values);
    int count = 0;
    values[strlen(values)-1] = '\0';
    char *saveptr = NULL;
    char *token = strtok_r(values, ",", &saveptr);
    while(token != NULL)
    {
        if(count > 3)
        {
            perror("La fonction calcul attend trois arguments : opérateur nb1 et nb2.");
            return -1;
        }

        if(count == 0)
        {
            sscanf(token, "\"%s\"", token);
            token[strlen(token)-1] = '\0';

            //if the message com from the server
            if(from == 0)
            {
                int nb = atoi(token);
                if(nb == 0)
                {
                    perror("Erreur de réponse au calcul");
                    return -1;
                }
            }
            else
            {
                if(checkOperators(token) != 0)
                {
                    perror("Mauvais opérateur de calcul.");
                    return -1;
                }
            }
        }
        else
        {
            if (from == 0)
            {
                perror("On attend qu'une réponse du serveur pour le calcul.");
                return -1;
            }
            sscanf(token, "\"%s\"", token);
            token[strlen(token)-1] = '\0';
            int nb = atoi(token);
            if(nb == 0)
            {
                perror("Les deuxièmes et troisièmes arguments attendus pour la fonction calcul sont des nombres.");
                return -1;
            }
        }
        values = NULL;
        token = strtok_r(values, ",", &saveptr);
        count += 1;
    }

    return 0;
}

int checkCouleurs(char *values, int from)
{
    sscanf(values, "[%s]", values);
    int count = 0;
    values[strlen(values)-1] = '\0';
    char *saveptr = NULL;
    char *token = strtok_r(values, ",", &saveptr);
    while(token != NULL)
    {
        if (count == 0)
        {
            if(from == 0)
            {
                sscanf(token, "\"%s\"", token);
                token[strlen(token)-1] = '\0';
                int nbCouleurs = atoi(token);
                if(nbCouleurs != 0)
                {
                    perror("Mauvaise réponse serveur pour l'instruction couleursssss");
                    return -1;
                }
            }
            else
            {
                sscanf(token, "\"%s\"", token);
                token[strlen(token)-1] = '\0';
                int nbCouleurs = atoi(token);
                if(nbCouleurs == 0)
                {
                    perror("Le premier argument attendu pour la fonction couleurs est un nombre.");
                    return -1;
                }
            }

        }
        values = NULL;
        token = strtok_r(values, ",", &saveptr);
        count += 1;
    }

    return 0;
}

int checkOperators(char *token)
{
    char plus[10] = "+";
    char moins[10] = "-";
    char div[10] = "/";
    char fois[10] = "*";
    if(strcmp(token, plus) == 0)
    {
        return 0;
    } else if(strcmp(token, moins) == 0)
    {
        return 0;
    } else if(strcmp(token, div) == 0)
    {
        return 0;
    } else if(strcmp(token, fois) == 0)
    {
        return 0;
    }

    return -1;
}

int valideCode(char *index, char *code)
{
    if(valideIndex(code, '"', '\0') != 0 || valideIndex(index, '"', '\0') != 0)
    {
        return -1;
    }

    return 0;
}

int valideIndex(char *index, char cmp, char cmpEnd)
{
    if(cmpEnd == '\0')
    {
        cmpEnd = cmp;
    }

    if(index[0] != cmp || index[strlen(index)-1] != cmpEnd)
    {
        return -1;
    }

    return 0;
}

int valideValeurs(char *valideJson)
{
    char *json = valideJson;
    char index[500] = "", valeurs[1000]="", *cleanValues;
    char *saveptr = NULL;
    //here we take the first line of the json (which correspond to '{')
    char *token = strtok_r(json, "\n", &saveptr);
    json=NULL;
    //here we take the second line of the json (which correspond to '"code" : "message"')
    token = strtok_r(json, "\n", &saveptr);
    json=NULL;
    //here we take the third line of the json (which correspond to '"valeurs" : ["valeur1", "valeur2"]')
    token = strtok_r(json, "\n", &saveptr);
    json=NULL;
    //this line 'token value is what  we want to control
    sscanf(token, "%s : %s", index, valeurs);

    //check if global syntax is correct for values '"valeurs" : [...]'
    if(valideIndex(index, '"', '\0') != 0 || valideIndex(valeurs, '[', ']') != 0)
    {
        return -1;
    }

    sscanf(valeurs, "[%s]", index);
    index[strlen(index)-1] = '\0';
    cleanValues = index;

    token = strtok_r(cleanValues, ",", &saveptr);
    while(token != NULL)
    {
        if(valideIndex(token, '"', '\0') != 0 )
        {
            return -1;
        }
        cleanValues = NULL;
        token = strtok_r(cleanValues, ",", &saveptr);
    }
    return 0;
}
