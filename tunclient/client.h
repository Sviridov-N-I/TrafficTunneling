#ifndef CLIENT_H_
#define CLIENT_H_


#include "private_client_variables.h"


// init a conection and return a socket descriptoin
Client_resource* dns_tun_client_init(char *server_ip,int port);

// return character position in line
int find_char(char* str, int n, char s,int startpos);

// extract a string(sorce name) from line from a file
char* extract_str(char* str);

// execute DNS quey
//int generate_dns_query(int sock,char *input_file,char *output_file);
int generate_dns_query(Client_resource *resource);

// free all resource
void dns_tun_client_deinit(Client_resource *resource);

#endif /*#ifndef CLIENT_H_*/
