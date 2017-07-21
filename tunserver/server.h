#ifndef SERVER_H_
#define SERVER_H_

//#include "server.c"
#include <jansson.h>//jansson.h>
#include <jansson_config.h>

#define PORT 8888
#define BUL_LEN 2000



#define T_A 1
#define T_TXT 16

int dns_tun_server_init(int port);

json_t* dnsquery (unsigned char* , int);

void ChangetoDnsNameFormat (unsigned char*,unsigned char*);

#endif /* SERVER_H_*/
