#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#include <jansson.h>//jansson.h>
#include <jansson_config.h>
#include "server_dns_query.h"


#define PORT 8888
#define BUL_LEN 2000


#define T_A 1
#define T_TXT 16

int dns_tun_server_init(int port)
{
  int socket_desc;
  struct sockaddr_in server;

  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
  {
      printf("Could not create socket");
      return -1;
  }
  puts("Socket created");

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
      perror("bind failed. Error");
      close(socket_desc);
      return 1;
  }
  puts("bind done");

  return socket_desc;
}
