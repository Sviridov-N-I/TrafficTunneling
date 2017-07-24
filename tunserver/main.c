
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
#include <signal.h>


void handle_ctrl_c(int sig)
{
  // there will be free resource
  printf("\nCTRL+C was pressed\n");
  exit(EXIT_SUCCESS);
}

int main(int argc , char *argv[])
{
    (void)signal(SIGINT, handle_ctrl_c);

    int queue_length = 3;
    Server_resource * resource = dns_tun_server_init(8888,queue_length);
    if(resource==NULL) { return -1; }

    set_number_of_threads(resource,200);

    if( processing(resource) == -1) { return -1; }
    dns_tun_server_deinit(resource);


    return 0;
}
