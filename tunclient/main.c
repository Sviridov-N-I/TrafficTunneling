/*#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#include <jansson.h>
#include <jansson_config.h>
*/

//#include </home/nikolay/Desktop/Tunneling/TrafficTunneling/protocol/protocol.h>

#include "client.h"





int main()
{

  int sock = dns_tun_client_init("127.0.0.1",8888);
  if(sock==-1) { return -1; }
  if(generate_dns_query(sock,"input","outup")==-1)  {return -1;}
  return 0;

}
