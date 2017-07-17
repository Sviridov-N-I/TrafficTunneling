
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

#define CURRENT_VERSION 1
#define NUMBER_FIELD 3 // for query message

json_t* dnsquery (unsigned char* , int);
void ChangetoDnsNameFormat (unsigned char*,unsigned char*);
unsigned char* ReadName (unsigned char*,unsigned char*,int*);



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
        printf("\ntxt_data_len = %d\n",txt_data_len);
        cnt_extract_rec++;//pass to data

        printf("\nrecord %d:\n",k+1); // useful use 'k'
        buf_for_add_in_reply = (char*)malloc(txt_data_len*sizeof(char)+1);
        memset(buf_for_add_in_reply,0,txt_data_len*sizeof(char)+1);

        for(int it = 0; it < txt_data_len;it++) {
          printf("%c", buf[cnt_extract_rec + it]);
          buf_for_add_in_reply[it]=buf[cnt_extract_rec + it];
        }
    //    printf("\n%s\n",buf_for_add_in_reply);
        printf("\nreply->current_count = %d\n",reply->current_count);
        reply_add_str(reply,buf_for_add_in_reply);
        printf("reply->current_count = %d\n",reply->current_count);

        cnt_extract_rec+=txt_data_len;
      }

      else if(buf[cnt_extract_rec] == 0 && buf[cnt_extract_rec+1] == T_A) // check type reply
      {

        cnt_extract_rec+=2;

          cnt_extract_rec+=6;// skip class and TTL
          int size_ip =(int)buf[cnt_extract_rec+1];
          cnt_extract_rec+=2; // skip size ip
      //    printf("\nSIZE IP = %d\n",size_ip);

          printf("\nrecord %d:\n",k+1); // useful use 'k'
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
          printf("%d", buf[cnt_extract_rec + it]);
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
    printf("JSON:\n");


    return reply_to_jsonformat(reply);






    dns = (struct DNS_HEADER*) buf;

    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];


    // reading answers//
    stop=0;
    for(i=0;i<ntohs(dns->ans_count);i++)
    {
        answers[i].name=ReadName(reader,buf,&stop);
        reader = reader + stop;

        answers[i].resource = (struct R_DATA*)(reader);
        reader = reader + sizeof(struct R_DATA);


        if(ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
        {
            answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));

            for(j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
            {
                answers[i].rdata[j]=reader[j];
            }

            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';


            reader = reader + ntohs(answers[i].resource->data_len);
        }
        else
        {
            answers[i].rdata = ReadName(reader,buf,&stop);
            reader = reader + stop;
        }
    }

    json_t  *DNSreply = json_array(); // for trasfer to client
    json_array_append_new(DNSreply,json_integer(CURRENT_VERSION));
    json_array_append_new(DNSreply,json_integer(query_type)); // какой запрос, такой и ответ
    json_array_append_new(DNSreply,json_integer((int)ntohs(dns->ans_count)));

//    printf("\n\nCOUNT==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(DNSreply, 2)));

//    printf("\nAnswer Records : %d \n16" , ntohs(dns->ans_count) );
    for(i=0 ; i < ntohs(dns->ans_count) ; i++)
    {
    //    printf("new iteration %d\t",i);
    //    printf("Type = %u\t",ntohs(answers[i].resource->type));

        if( ntohs(answers[i].resource->type) == T_A) //IPv4 address
        {
    //      printf("ANSWER\n");

            long *p;
            p=(long*)answers[i].rdata;
            a.sin_addr.s_addr=(*p); //working without ntohl
//!!!!!!!!!!!!!!!            printf("has IPv4 address : %s\n",inet_ntoa(a.sin_addr));
            json_array_append_new(DNSreply,json_string(inet_ntoa(a.sin_addr)));
        //    printf("\n   JANSSON==->     %s", json_string_value(json_array_get(DNSreply, 3+i)));

        }

        char t[2000];
//!!!!!!!!!!!!!!!        printf("type = %d\t",ntohs(answers[i].resource->type));

        if(ntohs(answers[i].resource->type)==T_TXT || ntohs(answers[i].resource->type)==4096) // TXT record
        {

//!!!!!!!!!!!!!!!             printf("type = %d\n",ntohs(answers[i].resource->type));
    /*      printf("class = %d\t",ntohs(answers[i].resource->_class));
          printf("ttl = %d\t",ntohs(answers[i].resource->ttl));
          printf("data_len = %d\t",ntohs(answers[i].resource->data_len));
          printf("\n\t\t rdata = %s\n",answers[i].rdata);*/
/*          printf("data_len = %d\n",answers[i].resoure->data_len);*/
//!!!!!!!!!!!!!!!               printf("  TXT resoure[%d] : %s",i,answers[i].rdata);
          //  printf("\tstrlen = %d",strlen(answers[i].rdata));
             strcpy(t,answers[i].rdata);
        //    printf("\t t=%s\n",t);
             json_array_append_new(DNSreply,json_string(t));
        //    printf("\n   JANSSON==->     %s", json_string_value(json_array_get(DNSreply, 3+i)));
//!!!!!!!!!!!!!!!                 printf("\n");
        }




    }


    return DNSreply;
}


u_char* ReadName(unsigned char* reader,unsigned char* buffer,int* count)
{
    unsigned char *name;
    unsigned int p=0,jumped=0,offset;
    int i , j;

    *count = 1;
    name = (unsigned char*)malloc(256);

    name[0]='\0';

    //read the names in 3www6google3com format
    while(*reader!=0)
    {
        if(*reader>=192)
        {
            offset = (*reader)*256 + *(reader+1) - 49152;
            reader = buffer + offset - 1;
            jumped = 1;
        }
        else
        {
            name[p++]=*reader;
        }

        reader = reader+1;

        if(jumped==0)
        {
            *count = *count + 1;
        }
    }

    name[p]='\0';
  //  printf("name: %s\n",name);
    if(jumped==1)
    {
        *count = *count + 1;
    }
  //  printf("name before: %s\n",name);
  //  return name;
  //  printf("\n for: ");

    for(i=0;i<(int)strlen((const char*)name);i++)
    {
        p=name[i];
        //printf("name[i]: %c\t",name[i]);
      //  printf("p: %d\n",p);

        for(j=0;j<(int)p;j++)
        {
          //  printf("%c",name[i]);
            if(name[i]==6) { break; printf("(%d)",name[i]);}
              if(name[i]==2) {break; printf("(%d)",name[i]);}

            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    printf("\n");
    name[i-1]='\0';
  //  printf("name after:   %s\n",name);
    return name;
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
