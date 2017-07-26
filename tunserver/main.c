
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

//Server_resource *resource;

void print_error()
{
  printf("\n***********************************************\n");
  printf("*\tThis is DNS tunneling system          *\n");
  printf("*\tEnter the following parameters:       *\n");
  printf("*\t-q is the queue length                *\n");
  printf("*\t-t is the number of threads           *\n");
  printf("*\tExample: \"./server -q 15 -t 10\"       *\n");
  printf("***********************************************\n\n");
}

int main(int argc , char *argv[])
{
  int rez=0;
  char* char_queue_length;
  char* char_n_threds;
  int help = 0;
  while ( (rez = getopt(argc,argv,"q:t:h")) != -1)
  {
    switch (rez)
    {
    case 'q': char_queue_length =(char*)optarg;
      break;
    case 't': char_n_threds = (char*)optarg;
      break;
    case 'h': help = 1;
        break;
    };
  };

  if(help) { print_error(); return 0; }



  if(char_n_threds==NULL || char_queue_length==NULL) {
    printf("\t Error.\n");
    print_error();
    return 0;
  }

  int queue_length = atoi(char_queue_length);
  int n_threds = atoi(char_n_threds);

  Server_resource *resource = dns_tun_server_init(8888,queue_length);
  if(resource==NULL) { return -1; }

  set_number_of_threads(resource,n_threds);

  if( processing(resource) == -1) { return -1; }
  dns_tun_server_deinit(resource);


    return 0;
}
