#ifndef CLIENT_H_
#define CLIENT_H_

//#include<stdio.h>
//#include<string.h>
//#include<sys/socket.h>
//#include<arpa/inet.h>

//#include <jansson.h>
//#include <jansson_config.h>


#include "private_variables.h"
#include "client.c"


// return a socket descriptoin
int dns_tun_client_init(char *server_ip,int port);

// return character position in line
int find_char(char* str, int n, char s,int startpos);

// extract a string(sorce name) from line from a file
char* extract_str(char* str);

json_t* create_dns_query(int version, int type_query, char* resourse_name);

int  generate_dns_query(int sock,char *input_file,char *output_file);


#endif /*#ifndef CLIENT_H_*/
