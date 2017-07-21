#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#include <jansson.h>//jansson.h>
#include <jansson_config.h>
//#include "server_dns_query.h"
#include <protocol.h>

///////////////////////////////////////////////////////////////////////////////
#include "private_server_variables.h"
// query type





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
        memset(buf_for_add_in_reply,0,txt_data_len*sizeof(char)+1);

        for(int it = 0; it < txt_data_len;it++) {
          printf("%c", buf[cnt_extract_rec + it]);
          buf_for_add_in_reply[it]=buf[cnt_extract_rec + it];
        }
        reply_add_str(reply,buf_for_add_in_reply);
        printf("\n");

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
          memset(buf_for_add_in_reply,0,16);


          int it = 0;
          sprintf(buf_for_add_in_reply, "%d.%d.%d.%d", buf[cnt_extract_rec],
                                      buf[cnt_extract_rec + 1],
                                      buf[cnt_extract_rec + 2],
                                      buf[cnt_extract_rec + 3]);
          for(; it < size_ip-1;it++)
          {
            printf("%d.", buf[cnt_extract_rec + it]);
          }
          printf("%d\n", buf[cnt_extract_rec + it]);
        //  printf("\n%s\n", buf_for_add_in_reply);
          reply_add_str(reply,buf_for_add_in_reply);
          cnt_extract_rec+=size_ip;
      }
      else
      {
        // message about error
      }
    }
    printf("\n");


    return reply_to_jsonformat(reply);

}




///////////////////////////////////////////////////////////////////////////////
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
