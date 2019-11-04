#ifndef __VALIDATEUR_H__
#define __VALIDATEUR_H__

#define PORT 8089

int valideJson(char *json, int from);

int valideCode(char *index, char *code);

int valideValeurs(char *json);

int valideIndex(char *index, char cmp, char cmpEnd);

int dataConsistency(char *code, char *valideJson, int from);

int checkMessage(char *values);

int checkName(char *values);

int checkCalcul(char *values, int from);

int checkCouleurs(char *values, int from);

int checkOperators(char *token);

#endif
