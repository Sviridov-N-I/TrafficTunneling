
#include "server.h"

#include <jansson.h>
#include <jansson_config.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <protocol.h>
#include "private_server_variables.h"


int main(int argc , char *argv[])
{


  Server_resource *resource = dns_tun_server_init(8888,0);
  if(resource==NULL) { return -1; }

  set_number_of_threads(resource,N_THREADS);

  if( processing(resource) == -1) { return -1; }
  dns_tun_server_deinit(resource);


    return 0;
}
