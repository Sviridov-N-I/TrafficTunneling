#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

#include <jansson.h>
#include <jansson_config.h>
#include <protocol.h>
#include <pthread.h> //for threading , link with lpthread
#include <signal.h>


#include "private_server_variables.h"

Server_resource *global_resource;


void handle_ctrl_c(int sig);


Server_resource* dns_tun_server_init(int port, int n_listen)
{
  int socket_desc;
  struct sockaddr_in server;

  (void)signal(SIGINT, handle_ctrl_c); // signal recording


  socket_desc = socket(AF_INET , SOCK_STREAM , 0); //SO_REUSEADDR);
  if (socket_desc == -1)
  {
      printf("Could not create socket");
      return NULL;
  }
  #ifdef DEBUG
  puts("Socket created");
  #endif

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
      perror("bind failed. Error");
      close(socket_desc);
      return NULL;
  }

  #ifdef DEBUG
  puts("bind done");
  #endif

  Server_resource *resource = (Server_resource*)malloc(sizeof(Server_resource));
  if(resource==NULL)
  {
    printf("malloc failed\n");
    return NULL;
  }
  resource->sock=socket_desc;

  listen(resource->sock , n_listen);
  global_resource = resource;

  return resource;
}


void dns_tun_server_deinit(Server_resource *resource)
{
  if(resource==NULL) { return; }
  close(resource->sock);
/*  if(resource->mas_sock != NULL)
  {
    for(int i=0;i<N_THREADS;i++) { close(resource->mas_sock[i]); }
  //  free(resource->mas_sock);
}*/
  free(resource);
}


void handle_ctrl_c(int sig)
{
  dns_tun_server_deinit(global_resource);
  printf("\nResources were released\n");
  exit(EXIT_SUCCESS);
}



void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host)
{
    int lock = 0 , i;
    strcat((char*)host,".");

    for(i = 0 ; i < strlen((char*)host) ; i++)
    {
        if(host[i]=='.')
        {
            *dns++ = i-lock;
            for(;lock<i;lock++)
            {
                *dns++=host[lock];
            }
            lock++;
        }
    }

    *dns++='\0';

}


json_t* dnsquery(unsigned char *host , int query_type)
{
    unsigned char buf[BUFSIZE],*qname,*reader;
    int i , j , stop , s;

    struct sockaddr_in a;

    struct RES_RECORD answers[50],auth[20],addit[20];
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;


    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(DNS_SERVER_IP);

    dns = (struct DNS_HEADER *)&buf;

    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0;
    dns->opcode = 0;
    dns->aa = 0;
    dns->tc = 0;
    dns->rd = 1;
    dns->ra = 0;
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];

    ChangetoDnsNameFormat(qname , host);

    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)]; //fill it

    qinfo->qclass = htons(1);
    qinfo->qtype = htons( query_type );


    if( sendto(s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        perror("sendto failed");
        return NULL;
    }
    memset(buf,0,65536);
    if(recvfrom (s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
    {
        perror("recvfrom failed");
        return NULL;
    }

    Reply *reply = ( Reply*)malloc(sizeof(Reply)); // not init yet

    if(reply==NULL)
    {
      printf("malloc failed\n");
      return NULL;
    }


    memset(reply,0,sizeof(Reply));



    int buffer_counter=13; // start position(there is begin reple name)

    for( ; ;buffer_counter++){
      if(buf[buffer_counter]==0) break; //skip the soure name
    }
    buffer_counter+=5;// now buffer_counter poiters to C0(name+5)


    int cnt_extract_rec = buffer_counter;
    int txt_data_len=0;

    if(buf[cnt_extract_rec+2] == 0 && buf[cnt_extract_rec+3] == T_TXT) {reply_init(reply,100,T_TXT);}
    if(buf[cnt_extract_rec+2] == 0 && buf[cnt_extract_rec+3] == T_A) {reply_init(reply,100,T_A);}

    for(int k=0;k<65535;k++)
    {
      if(buf[cnt_extract_rec+1]==0) { break;}
      cnt_extract_rec+=2;// skip the name(C0 & 0C)
      char* buf_for_add_in_reply;

      if(buf[cnt_extract_rec] == 0 && buf[cnt_extract_rec+1] == T_TXT) // check type reply
      {

        cnt_extract_rec+=2; //skip type reply
    //    printf("TXT TYPE\n");
        cnt_extract_rec+=6;// skip class and TTL
        cnt_extract_rec+=2;// skip data lengh
        txt_data_len = buf[cnt_extract_rec];
    //    printf("\ntxt_data_len = %d\n",txt_data_len);
        cnt_extract_rec++;//pass to data

      //  printf("\nrecord %d:\n",k+1); // useful use 'k'
        buf_for_add_in_reply = (char*)malloc(txt_data_len*sizeof(char)+1);
        if(buf_for_add_in_reply==NULL)
        {
          printf("malloc failed\n");
          return NULL;
        }


        memset(buf_for_add_in_reply,0,txt_data_len*sizeof(char)+1);

        for(int it = 0; it < txt_data_len;it++)
        {
          #ifdef DEBUG
          printf("%c", buf[cnt_extract_rec + it]);
          #endif
          buf_for_add_in_reply[it]=buf[cnt_extract_rec + it];
        }
        reply_add_str(reply,buf_for_add_in_reply);

        #ifdef DEBUG
        printf("\n");
        #endif

        cnt_extract_rec+=txt_data_len;
      }

      else if(buf[cnt_extract_rec] == 0 && buf[cnt_extract_rec+1] == T_A) // check type reply
      {

        cnt_extract_rec+=2;

          cnt_extract_rec+=6;// skip class and TTL
          int size_ip =(int)buf[cnt_extract_rec+1];
          cnt_extract_rec+=2; // skip size ip
      //    printf("\nSIZE IP = %d\n",size_ip);

        //  printf("\nrecord %d:\n",k+1); // useful use 'k'
          buf_for_add_in_reply = (char*)malloc(16);

          if(buf_for_add_in_reply==NULL)
          {
            printf("malloc failed\n");
            return NULL;
          }

          memset(buf_for_add_in_reply,0,16);


          int it = 0;
          sprintf(buf_for_add_in_reply, "%d.%d.%d.%d", buf[cnt_extract_rec],
                                      buf[cnt_extract_rec + 1],
                                      buf[cnt_extract_rec + 2],
                                      buf[cnt_extract_rec + 3]);
          #ifdef DEBUG
          for(; it < size_ip-1;it++)
          {
            printf("%d.", buf[cnt_extract_rec + it]);
          }

          printf("%d\n", buf[cnt_extract_rec + it]);
          #endif
        //  printf("\n%s\n", buf_for_add_in_reply);
          reply_add_str(reply,buf_for_add_in_reply);
          cnt_extract_rec+=size_ip;
      }
      else
      {
        // message about error
      }
    }
  //  printf("\n");

    reply->Type = query_type;
    return reply_to_jsonformat(reply);

}


void set_number_of_threads(Server_resource *resource, int n)
{
  resource->number_of_threads = n;

}

void* function_of_client_service(void *socket_desc);


int processing(Server_resource *resource)
{


  int N = resource->number_of_threads;
  pthread_t *mas_thread_id=(pthread_t*)malloc(N*sizeof(pthread_t));
  if(mas_thread_id==NULL)
  {
    printf("malloc failed\n");
    return -1;
  }

  memset(mas_thread_id,0,N*sizeof(pthread_t));

  int *new_sock = malloc(sizeof(int));
  if(new_sock==NULL)
  {
    printf("malloc failed\n");
    return -1;
  }

  *new_sock = resource->sock;

  for(int i = 0; i < N ; i++)
  {
    if( pthread_create( &mas_thread_id[i] , NULL ,  function_of_client_service , (void*) new_sock) < 0)
    {
        perror("could not create thread");
        return 1;
    }
  }

  for(int i = 0;i<N;i++)
  {
    pthread_join( mas_thread_id[i] , NULL);
  }

  free(new_sock);


  return 0;
}


void* function_of_client_service(void *socket_desc)
{
  int sock = *(int*)socket_desc;



  int  client_sock , c , read_size;
  struct sockaddr_in client;
  char client_message[BUL_LEN];

  json_t* buf_json = NULL;
  json_t* jDNS_reply = NULL;
  char* ToChar = NULL;
  Query *query = ( Query*)malloc(sizeof(Query));
  if(query==NULL)
  {
    printf("malloc failed\n");
    return 0;
  }

  while(1)
  {
    #ifdef DEBUG
    puts("Waiting for incoming connections...\n");
    #endif

    c = sizeof(struct sockaddr_in);

    client_sock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
    //    close(client_sock);
        dns_tun_server_deinit(global_resource);
        exit(EXIT_FAILURE);
        //  goto close_sock;
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
    //  printf("sourse: %s\t before query\n",source_name_of_query(query));
      jDNS_reply = dnsquery(source_name_of_query(query) ,type_of_query(query));
    //  printf("sourse: %s\t after query\n\n\n",source_name_of_query(query));


        if(jDNS_reply==NULL)
        {
          printf("\ndnsquery failed\n");
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
          #ifdef DEBUG
          puts("Client disconnected\n\n\n");
          close(client_sock);
          #endif
      }
      else if(read_size == -1)
      {
          perror("recv failed");
      }
  }
  return 0;
  close_sock:
  close(client_sock);
  //dns_tun_server_deinit(resource);
  return 0;
}
