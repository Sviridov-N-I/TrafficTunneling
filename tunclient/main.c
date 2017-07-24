
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>

#include <protocol.h>

#include "client.h"
#include "private_client_variables.h"



//  ./main -a 127.0.0.1 -p 8888 -s input -d output

void print_error()
{
  printf("\n********************************************************************\n");
  printf("*\tThis is DNS tunneling system                               *\n");
  printf("*\tEnter the following parameters:                            *\n");
  printf("*\t-a server IP-address s                                     *\n");
  printf("*\t-p Port                                                    *\n");
  printf("*\t-s input(source) file                                      *\n");
  printf("*\t-d destination(output) file                                *\n");
  printf("*\tExample: \"./main -a 127.0.0.1 -p 8888 -s input -d output\"  *\n");
  printf("********************************************************************\n\n");
}


int main(int argc, char *argv[])
{

  int rez=0;
  int help=0;
  opterr=0;
  char *source_file=NULL,// = "input",
       *destination_file=NULL,// = "ouput",
       *ip_address=NULL,//= "127.0.0.5",
       *port=NULL;// = "8888";

//  printf("Source file: !%s!\n",source_file);


  while ( (rez = getopt(argc,argv,"a:p:s:d:h")) != -1)
  {
    switch (rez)
    {
    case 'a': ip_address=(char*)optarg;
      break;
    case 'p': port=(char*)optarg;
      break;
    case 's': source_file = (char*)optarg;
      break;
    case 'd': destination_file = (char*)optarg;
        break;
    case 'h': help=1;
        break;
    };
  };


  if(help) { print_error(); return 0; }



  if(source_file==NULL || destination_file==NULL || ip_address==NULL || port == NULL) {
    printf("\t Input error \n");
    print_error();
    return 0;
  }


  Client_resource *resource = dns_tun_client_init(ip_address,atoi(port));
  resource->input_file = source_file;
  resource->output_file = destination_file;

  if(resource==NULL) { return -1; }
  if(generate_dns_query(resource)==-1)  {return -1;}
  dns_tun_client_deinit(resource);


  return 0;

}
