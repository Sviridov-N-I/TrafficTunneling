
#include "server.h"






int main(int argc , char *argv[])
{
    int  client_sock , c , read_size;
    struct sockaddr_in client;

    int socket_desc = dns_tun_server_init(8888);
    if(socket_desc==-1) { return -1; }

    json_t* buf_json = NULL;
    json_t* jDNS_reply = NULL;
    char* ToChar = NULL;

    char client_message[BUL_LEN];
    Query *query = ( Query*)malloc(sizeof(Query));


      listen(socket_desc , 3);
      while(1)
      {
        puts("Waiting for incoming connections...\n");
        c = sizeof(struct sockaddr_in);

        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            goto close_sock;
        }

        while( (read_size = recv(client_sock , client_message , BUL_LEN , 0)) > 0 )
        {
          memset(query,0,sizeof(Query));
          buf_json = json_loads(client_message,0,NULL);
          if(buf_json==NULL)
          {
            printf("JSON Error\n");
            goto close_sock;
          }
          query = jsonformat_to_query(buf_json);
          jDNS_reply = dnsquery(source_name_of_query(query) ,type_of_query(query));

            if(jDNS_reply==NULL)
            {
              printf("Error\n");
              goto close_sock;
            }
            ToChar=json_dumps(jDNS_reply,0);
            strcpy(client_message,ToChar);

            write(client_sock , client_message , BUL_LEN);
            free(buf_json);
            strcpy(client_message,"");
            memset(client_message,0,BUL_LEN); // for(int it=0;it<BUL_LEN;it++){ client_message[it]='\0'; }

        }

        if(read_size == 0)
        {
            puts("Client disconnected\n\n\n");
        }
        else if(read_size == -1)
        {
            perror("recv failed");
        }
    }
    close_sock:
    close(socket_desc);


    return 0;
}
