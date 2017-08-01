#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "private_client_variables.h"
#include <jansson.h>
#include <jansson_config.h>
#include <unistd.h>

#include <protocol.h>
#include <pthread.h> //for threading , link with lpthread
#include <signal.h>

Client_resource *global_resource;

pthread_mutex_t f_read;
pthread_mutex_t f_write;

void handle_ctrl_c(int sig);

//#define DEBUG




Client_resource* dns_tun_client_init(char *server_ip,int port)
{
  (void)signal(SIGINT, handle_ctrl_c); // signal recording

  pthread_mutex_init(&f_read, NULL);
  pthread_mutex_init(&f_write, NULL);


  int sock;
  int *mas_sock = (int*)malloc(N_THREADS*sizeof(int));
  if(mas_sock==NULL) { return NULL; }
  memset(mas_sock, 0, sizeof(N_THREADS*sizeof(int)));

  struct sockaddr_in server;

  for(int i=0;i<N_THREADS;i++)
  {
    mas_sock[i] = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        return NULL;
    }

    #ifdef DEBUG
    puts("Socket created");
    #endif
  }

  server.sin_addr.s_addr = inet_addr(server_ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);


  Client_resource *resource = ( Client_resource*)malloc(sizeof(Client_resource));
  if(resource==NULL) { printf("malloc failed\n"); return NULL; }
  memset(resource, 0, sizeof(Client_resource));
  global_resource = resource;


  resource->server = server;
  resource->mas_sock=mas_sock;

  return resource;
}



void* thread_dns_query(void *resource);

int generate_dns_query(Client_resource *resource)
{

  char * input_file = resource->input_file;
  char * output_file = resource->output_file;

  FILE *f_in = fopen(input_file,"r");
  FILE *f_out = fopen(output_file, "w");
  if(f_in==NULL ) { printf("File not found\nError\n");  goto done;  }
  if(f_out==NULL ){ printf("File error\n"); goto close_input_file;  }

  resource->input_file_descr = f_in;
  resource->output_file_descr = f_out;


  pthread_t *mas_thread_id=(pthread_t*)malloc(N_THREADS*sizeof(pthread_t));
  if(mas_thread_id==NULL) { printf("malloc failed\n"); return -1; }
  memset(mas_thread_id, 0, N_THREADS*sizeof(pthread_t));

  Thread_resource **th_resource = (Thread_resource**)malloc(N_THREADS*sizeof(Thread_resource));
  if(th_resource==NULL) return -1;

  for(int i=0; i<N_THREADS; i++)
  {
    th_resource[i] = (Thread_resource*)malloc(sizeof(Thread_resource));
    th_resource[i]->server = resource->server;
    th_resource[i]->sock = resource->mas_sock[i];
    th_resource[i]->input_file_descr = f_in;
    th_resource[i]->output_file_descr = f_out;

    if( pthread_create( &mas_thread_id[i] , NULL ,  thread_dns_query , (void*) th_resource[i]) < 0)
    {
        perror("could not create thread");
        return 1;
    }
  }

  for(int i = 0;i<N_THREADS;i++)
  {
    pthread_join( mas_thread_id[i] , NULL);
  }

  return 0;

/*  close_files:
    fclose(f_out);*/
  close_input_file:
    fclose(f_in);
  done:
  return -1;

}

void* thread_dns_query(void *resource)
{
  Thread_resource *thread_resource = (Thread_resource*)resource;
  int sock = thread_resource->sock;
  FILE* f_in = thread_resource->input_file_descr;
  FILE* f_out = thread_resource->output_file_descr;

  char *buf_for_strtok_r;

  if (connect(sock , (struct sockaddr *)&(thread_resource->server), sizeof(struct sockaddr_in)) < 0)
  {
      perror("connect failed. Error");
      return 0;
  }

  char file_line[READ_FILE_BUF_SIZE];
  char *mnemonic_name, *query_to_char;
  char buf_for_transfer[MESSAGE_SIZE];
  char res_name[READ_FILE_BUF_SIZE];

  char split [10] = ", \n";
  char *istr;
  while(!feof(f_in))
  {
  //  printf("before read\n");
    pthread_mutex_lock(&f_read);

    if(fgets(file_line,100,f_in)==NULL)
    {
      pthread_mutex_unlock(&f_read);
    //  printf("after read\n");
      return 0;
    }
    else
    {
      pthread_mutex_unlock(&f_read);

    //  printf("after  read\n");

      memset(res_name,0,READ_FILE_BUF_SIZE);
      strcpy(res_name,file_line);

      istr = strtok_r (file_line,split,&buf_for_strtok_r); // extract type query
      if(istr == NULL) {printf("error format\n");return 0;}
      if(!strcmp(istr,"A") || !strcmp(istr,"TXT")) // if type query A or TXT
      {


        Query *query = ( Query*)malloc(sizeof(Query));
        if(query==NULL)
        {
          printf("malloc failed\n");
          return NULL;
        }


        if(!strcmp(istr,"A")) // A-record
        {
          istr = strtok_r (NULL,split,&buf_for_strtok_r); // extract resource name
          if(istr == NULL) {printf("error format\n");return 0;}
          mnemonic_name = istr;
          query_init(query, T_A, mnemonic_name);
        }
        if(!strcmp(istr,"TXT")) // TXT-record, because we have only 2 options
        {
          istr = strtok_r (NULL,split,&buf_for_strtok_r); // extract resource name
          if(istr == NULL) {printf("error format\n");return 0;}
          mnemonic_name = istr;
          query_init(query, T_TXT, mnemonic_name);

        }

         json_t *json_new_format = query_to_jsonformat(query);
         query_to_char = json_dumps(json_new_format,0);
         strcpy(buf_for_transfer,query_to_char);


         if( send(sock , buf_for_transfer , strlen(buf_for_transfer) , 0) < 0){
             puts("Send failed");
             return NULL;
         }

         char server_reply[2000];

         if( recv(sock , server_reply , 2000 , 0) < 0) {  puts("recv failed"); break; }



         json_t* reply_json = json_loads(server_reply,0,NULL);
         if(reply_json==NULL)
         {
           printf("JSON Error\n");
           return NULL;
         }
         Reply*  reply = jsonformat_to_reply(reply_json);

      //   printf("before write\n");
         pthread_mutex_lock(&f_write);

         #ifdef DEBUG
         printf("%s",res_name);
         #endif

         if(reply->current_count==0)
         {
           if(type_of_reply(reply)==T_A)  fputs("A, ",f_out);
           if(type_of_reply(reply)==T_TXT)  fputs("TXT, ",f_out);

           fputs(mnemonic_name,f_out);
           fputs(", information not found\n",f_out);
         }

         for(int i = 0;i<reply->current_count;i++)
         {
           if(type_of_reply(reply)==T_A)  fputs("A, ",f_out);
           if(type_of_reply(reply)==T_TXT)  fputs("TXT, ",f_out);

           fputs(mnemonic_name,f_out);
           fputs(", ",f_out);
           fputs( reply_pop_str(reply,i) , f_out);
           fputs("\n",f_out);

           #ifdef DEBUG
           printf(" %s\n", reply_pop_str(reply,i) );
           #endif
         }
      //   printf("\n\n");

         pthread_mutex_unlock(&f_write);
      //   printf("after write\n");
         fflush(f_out);

      }
      else
      {
        istr = strtok_r (file_line,split,&buf_for_strtok_r);

        #ifdef DEBUG
        printf("Error in \"%s\"\n\n",istr);
        #endif

        pthread_mutex_lock(&f_write);

        fputs("Invalid string format, \"",f_out);
        fputs(istr,f_out);
        fputs("\"\n",f_out);

        pthread_mutex_unlock(&f_write);


      }
    }


  }
}




void dns_tun_client_deinit(Client_resource *resource)
{
  if(resource==NULL) { return; }
  fclose(resource->input_file_descr);
  fclose(resource->output_file_descr);
  for(int i=0; i<N_THREADS; i++)
  {
    close(resource->mas_sock[i]);
  }
  free(resource);
}

void handle_ctrl_c(int sig)
{

  dns_tun_client_deinit(global_resource);
  printf("\nResources were released\n");
  exit(EXIT_SUCCESS);
}
