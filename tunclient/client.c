#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "private_client_variables.h"
#include <jansson.h>
#include <jansson_config.h>
#include <unistd.h>

#include <protocol.h>

#include <signal.h>

Client_resource *global_resource;


void handle_ctrl_c(int sig);






Client_resource* dns_tun_client_init(char *server_ip,int port)
{

  (void)signal(SIGINT, handle_ctrl_c); // signal recording

  int sock;
  struct sockaddr_in server;

  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1) {
      printf("Could not create socket");
      return NULL;
  }
  puts("Socket created");

  server.sin_addr.s_addr = inet_addr(server_ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
      perror("connect failed. Error");
      return NULL;
  }
  puts("Connected\n");

  Client_resource *resource = ( Client_resource*)malloc(sizeof(Client_resource));
  if(resource==NULL)
  {
    printf("malloc failed\n");
    return NULL;
  }
  memset(resource, 0, sizeof(Client_resource));
  resource->sock=sock;
  global_resource = resource;
  return resource;
}




int generate_dns_query(Client_resource *resource)
{
  int sock = resource->sock;
  char * input_file = resource->input_file;
  char * output_file = resource->output_file;

  FILE *f_in = fopen(input_file,"r");
  FILE *f_out = fopen(output_file, "w");
  if(f_in==NULL ){
    printf("File not found\\Error\n");
    goto done;
  }
  if(f_out==NULL ){
    printf("File not found\nError\n");
    goto close_input_file;
  }

  resource->input_file_descr = f_in;
  resource->output_file_descr = f_out;



  char file_line[READ_FILE_BUF_SIZE];
  char *mnemonic_name, *query_to_char;
  char buf_for_transfer[MESSAGE_SIZE];

  char split [10]=", \n";
  char *istr;

  while(!feof(f_in))
  {
    if(fgets(file_line,100,f_in)!=NULL)
    {
      printf("%s",file_line);
      istr = strtok (file_line,split); // extract type query

      if(!strcmp(istr,"A") || !strcmp(istr,"TXT")) // if type query A or TXT
      {


        Query *query = ( Query*)malloc(sizeof(Query));
        if(query==NULL)
        {
          printf("malloc failed\n");
          goto close_files;
        }


        if(!strcmp(istr,"A")) // A-record
        {
          istr = strtok (NULL,split); // extract resource name
          mnemonic_name = istr;
          query_init(query, T_A, mnemonic_name);
        }
        if(!strcmp(istr,"TXT")) // TXT-record, because we have only 2 options
        {
          istr = strtok (NULL,split); // extract resource name
          mnemonic_name = istr;
          query_init(query, T_TXT, mnemonic_name);

        }

         json_t *json_new_format = query_to_jsonformat(query);
         query_to_char = json_dumps(json_new_format,0);
         strcpy(buf_for_transfer,query_to_char);


         if( send(sock , buf_for_transfer , strlen(buf_for_transfer) , 0) < 0){
             puts("Send failed");
             goto close_files;
         }

         char server_reply[2000];
         if( recv(sock , server_reply , 2000 , 0) < 0) {  puts("recv failed"); break; }

         json_t* reply_json = json_loads(server_reply,0,NULL);
         if(reply_json==NULL)
         {
           printf("JSON Error\n");
           goto close_files;
         }
         Reply*  reply = jsonformat_to_reply(reply_json);

         if(reply->current_count==0){
           if(type_of_reply(reply)==T_A)  fputs("A, ",f_out);
           if(type_of_reply(reply)==T_TXT)  fputs("TXT, ",f_out);
           fputs(mnemonic_name,f_out);
           fputs(", infromation not found\n",f_out);
         }

         for(int i = 0;i<reply->current_count;i++)
         {
           if(type_of_reply(reply)==T_A)  fputs("A, ",f_out);
           if(type_of_reply(reply)==T_TXT)  fputs("TXT, ",f_out);
           fputs(mnemonic_name,f_out);
           fputs(", ",f_out);
           fputs( reply_pop_str(reply,i) , f_out);
           fputs("\n",f_out);
           printf(" %s\n", reply_pop_str(reply,i) );
         }
         printf("\n\n");


      }
      else
      {
        istr = strtok (file_line,split);

        printf("Error in \"%s\"\n\n",istr);
        fputs("Invalid string format, \"",f_out);
        fputs(istr,f_out);
        fputs("\"\n",f_out);

      }
    }


  }


  return 0;

  close_files:
    fclose(f_out);
  close_input_file:
    fclose(f_in);
  done:
    close(sock);
  return -1;

}





void dns_tun_client_deinit(Client_resource *resource)
{
  fclose(resource->input_file_descr);
  fclose(resource->output_file_descr);
  close(resource->sock);
  free(resource);
}

void handle_ctrl_c(int sig)
{
  dns_tun_client_deinit(global_resource);
  printf("\nResources were released\n");
  exit(EXIT_SUCCESS);
}
