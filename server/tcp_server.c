/*
    C socket server example
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#include </usr/include/jansson.h>//jansson.h>
#include <jansson_config.h>
#include </home/nikolay/Desktop/Tunneling/TrafficTunneling/server/server_dns_query.h>

#define PORT 8888
#define BUL_LEN 2000

#define CURRENT_VERSION 1
#define NUMBER_FIELD 3 // for query message

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

int main(int argc , char *argv[])
{
    printA();

    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[BUL_LEN];

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        return -1;
    }
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );

    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");

        return 1;
    }
    puts("bind done");

    listen(socket_desc , 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return -1;
    }
    puts("Connection accepted");

    json_error_t error;
    json_t* buf_json = NULL;
    char* buf_name;
    unsigned char hostname[50];



    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
        buf_json = json_loads(client_message,0,&error);
        if(buf_json==NULL) {printf("JSON Error"); return -1;}
        buf_name=read_dns_query_soure_name(buf_json);
        puts(buf_name);
        strcpy(hostname,buf_name);
        dnsquery(hostname , 1);

  /*      printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(buf_json, 0)));        printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(buf_json, 1)));        printf("==->%s\n", json_string_value(json_array_get(buf_json, 2)));*/

        write(client_sock , client_message , strlen(client_message));

    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;
}
