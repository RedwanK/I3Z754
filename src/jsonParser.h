#ifndef __JSONPARSER_H__
#define __JSONPARSER_H__

#define PORT 8089


//encode a string into a JSON format
int transformToJson(char* data);

//decode a string form a JSON
int decodeFromJson(char* jsonResponse, char* codePrev);

#endif
