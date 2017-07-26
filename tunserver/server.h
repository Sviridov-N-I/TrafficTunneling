#ifndef SERVER_H_
#define SERVER_H_

#include <jansson.h>
#include <jansson_config.h>
#include "private_server_variables.h"

#define PORT 8888



#define T_A 1
#define T_TXT 16

Server_resource* dns_tun_server_init(int port,int n_listen);

Server_resource* dns_tun_server_deinit(Server_resource *resource);

void set_number_of_threads(Server_resource *resource, int n);

int processing(Server_resource *resource);

json_t* dnsquery (unsigned char* , int);

void ChangetoDnsNameFormat (unsigned char*,unsigned char*);

#endif /* SERVER_H_*/
