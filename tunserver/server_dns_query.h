
#ifndef SERVER_DNS_QUERY_H_
#define SERVER_DNS_QUERY_H_

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include <jansson.h>//jansson.h>
#include <jansson_config.h>
#include </home/nikolay/Desktop/Tunneling/TrafficTunneling/protocol/protocol.h>


#define DNS_SERVER_IP "8.8.8.8"
#define BUFSIZE 65536
// query type
#define T_A 1
#define T_TXT 16



json_t* dnsquery (unsigned char* , int);
void ChangetoDnsNameFormat (unsigned char*,unsigned char*);



struct DNS_HEADER
{
    unsigned short id;

    unsigned char rd :1;
    unsigned char tc :1;
    unsigned char aa :1;
    unsigned char opcode :4;
    unsigned char qr :1;

    unsigned char rcode :4;
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1;
    unsigned char ra :1;

    unsigned short q_count:16;
    unsigned short ans_count:16;
    unsigned short auth_count:16;
    unsigned short add_count:16;
};

struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

#pragma pack(push, 2)
struct R_DATA
{
    unsigned short type:16;
    unsigned short _class:16;
    unsigned int ttl:32;
    unsigned short data_len:16;
};
#pragma pack(pop)

struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;
/*
int main( int argc , char *argv[])
{
    unsigned char hostname[50];

    strcpy(hostname,"google.com");
    dnsquery(hostname , 1);

    return 0;#include </home/nikolay/Desktop/Tunneling/TrafficTunneling/protocol/protocol.h>

}*/

json_t* dnsquery(unsigned char *host , int query_type)
{
    unsigned char buf[BUFSIZE],*qname,*reader;
    int i , j , stop , s;

    struct sockaddr_in a;

    struct RES_RECORD answers[50],auth[20],addit[20];
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

//    printf("Resolving %s" , host);

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

#endif /* SERVER_DNS_QUERY_H_ */
