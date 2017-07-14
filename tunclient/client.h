#ifndef CLIENT_H_
#define CLIENT_H_


#include "private_variables.h"
#include "client.c"


// init a conection and return a socket descriptoin
int dns_tun_client_init(char *server_ip,int port);

// return character position in line
int find_char(char* str, int n, char s,int startpos);

// extract a string(sorce name) from line from a file
char* extract_str(char* str);

// execute DNS quey 
int generate_dns_query(int sock,char *input_file,char *output_file);


#endif /*#ifndef CLIENT_H_*/
