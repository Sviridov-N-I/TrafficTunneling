

#include "client.h"

//#include </home/nikolay/Desktop/Tunneling/TrafficTunneling/protocol/protocol.h>

//  ./main -a 127.0.0.1 -p 8888 -s input -d output




int main(int argc, char *argv[])
{
  int rez=0;
  int help=0;
  opterr=0;
  char *source_file,// = "input",
       *destination_file,// = "ouput",
       *ip_address,//= "127.0.0.5",
       *port;// = "8888";

//  printf("Source file: !%s!\n",source_file);


  while ( (rez = getopt(argc,argv,"a:p:s:d:h")) != -1)
  {
    switch (rez)
    {
    case 'a': //printf("IP-address: %s\n",optarg);
      ip_address=(char*)optarg;
      break;
    case 'p': //printf("port: %s\n",optarg);
      port=(char*)optarg;
      break;
    case 's': //printf("Source file: %s\n",optarg);
      source_file = (char*)optarg;
      break;
    case 'd': //printf("Destination file: %s\n",optarg);
        destination_file = (char*)optarg;
        break;
    case 'h': //printf("Source file: %s\n",optarg);
        printf("\n**********************************************\n");
        printf("*\tThis is DNS tunneling system         *\n");
        printf("*\tEnter the following parameters:      *\n");
        printf("*\t-a server IP-address s               *\n");
        printf("*\t-p Port                              *\n");
        printf("*\t-s input(source) file                *\n");
        printf("*\t-d destination(output) file          *\n");
        printf("**********************************************\n\n");

        help=1;
        break;
    };
  };
  if(help) { return 0; }
  if(source_file==NULL || destination_file==NULL || ip_address==NULL){ printf("Input error \n"); return 0;}


  int sock = dns_tun_client_init(ip_address,atoi(port));
  if(sock==-1) { return -1; }
  if(generate_dns_query(sock, source_file, destination_file)==-1)  {return -1;}
  return 0;

}
