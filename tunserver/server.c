#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#include <jansson.h>//jansson.h>
#include <jansson_config.h>
#include </home/nikolay/Desktop/Tunneling/TrafficTunneling/tunserver/server_dns_query.h>

#define PORT 8888
#define BUL_LEN 2000

#define CURRENT_VERSION 1
#define NUMBER_FIELD 3 // for query message

#define T_A 1
#define T_TXT 16

char* read_dns_query_soure_name(json_t *query)
{
    size_t size = json_array_size(query);
    if(size != NUMBER_FIELD) // структура запроса содержит 3 поля, ПРОВЕРИТЬ
    {
      printf("\t The packege structure was broken\n");
      return NULL;
    }

    json_t *query_type = json_array_get(query, 0);
    if(json_typeof(query_type)!=JSON_INTEGER){ // если на второй позиции не число, то нет смысла его проверять
      printf("\t The packege structure was broken\n");
      return NULL;
    }
    else{
      if(json_integer_value(query_type)!=CURRENT_VERSION) {  printf("\tVersions aren't equal\n"); return NULL; }// проверка совместимости версий
    }

    return (char*)json_string_value(json_array_get(query, 2));
}


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

      return 1;
  }
  puts("bind done");

  return socket_desc;
}


int main(int argc , char *argv[])
{
    int  client_sock , c , read_size;
    struct sockaddr_in client;

    int socket_desc = dns_tun_server_init(8888);
    if(socket_desc==-1) { return -1; }

    json_error_t error;
    json_t* buf_json = NULL;
    json_t* jDNS_reply = NULL;
    char* buf_name;
    unsigned char hostname[50];
    int query_type;
    char* ToChar = NULL;


      listen(socket_desc , 3);
      while(1)
      {
      puts("Waiting for incoming connections...\n");
      c = sizeof(struct sockaddr_in);

      client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
      if (client_sock < 0)
      {
          perror("accept failed");
          return -1;
      }
  //    puts("Connection accepted");


  //    int packet_size=-1;
      char client_message[BUL_LEN];

      while( (read_size = recv(client_sock , client_message , BUL_LEN , 0)) > 0 )
      {
          printf("READ_SIZE= %d\n",read_size);
          buf_json = json_loads(client_message,0,&error);
          if(buf_json==NULL)
          {
            printf("JSON Error\n");
            close(socket_desc);
            return -1;
          }
          buf_name=read_dns_query_soure_name(buf_json);
          puts("");
          puts(buf_name); //priintf resourse name

          strcpy(hostname,buf_name);
          query_type=json_integer_value(json_array_get(buf_json, 1));
          printf("type = %d\n",query_type);
          jDNS_reply = dnsquery(hostname ,query_type);
      //    printf("\nsize=%d\n",json_array_size(jDNS_reply));
    /*      printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(buf_json, 0)));        printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(buf_json, 1)));        printf("==->%s\n", json_string_value(json_array_get(buf_json, 2)));*/
      //     printf("==->%s\n", json_integer_value(json_array_get(jDNS_reply, 2)));
          printf("N = %d\n",(int)json_integer_value(json_array_get(jDNS_reply, 2)));
          ToChar=json_dumps(jDNS_reply,0);

  //      strcpy(client_message,"");
          strcpy(client_message,ToChar);

          write(client_sock , client_message , BUL_LEN);//strlen(client_message));
          free(buf_json);
          strcpy(client_message,"");
          strcpy(hostname,"");
          for(int it=0;it<BUL_LEN;it++){
              client_message[it]='\0';
            }
      }

      if(read_size == 0)
      {
          puts("Client disconnected\n\n\n");
      //    fflush(stdout);
      }
      else if(read_size == -1)
      {
          perror("recv failed");
      }
    }
    close(socket_desc);


    return 0;
}
