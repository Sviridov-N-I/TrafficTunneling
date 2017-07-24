
#include "server.h"

#include <jansson.h>//jansson.h>
#include <jansson_config.h>
#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <protocol.h>
#include "private_server_variables.h"



int main(int argc , char *argv[])
{

    Server_resource * resource= dns_tun_server_init(8888);
    if(resource==NULL) { return -1; }
    if( processing(resource) == -1) { return -1; }
    dns_tun_server_deinit(resource);


    return 0;
}
