#ifndef __TEST_H__
#define __TEST_H__

#define PORT 8089

int server_main();
int client_main(char *message, char *assertMessage);
int assertTrue(char* data, char* answer) ;
#endif