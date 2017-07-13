#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <jansson.h>
#include <jansson_config.h>
#include "privat_struct.h"


void query_init(Query *query, int type_query,char *source_name)
{
  query->Type = type_query;
  query->source_name = source_name;
}

void reply_init( Reply *reply, int N,int type_query)
{
  reply->current_count=0;
  reply->N=N;
  reply->Type=type_query;
  reply->mas_str=(char**)malloc(N*sizeof(void*));
}

int reply_add_str(Reply *reply, char *str)
{
  if(reply->N == reply->current_count) {return -1;}
  int cnt=reply->current_count++;
  reply->mas_str[cnt] = str;
  return 0;
}


json_t* query_to_jsonformat(Query *query)
{
  json_t  *dns_json_query = json_array();

  json_array_append_new(dns_json_query,json_integer(CURRENT_VERSION));
  json_array_append_new(dns_json_query,json_integer(query->Type));
  json_array_append_new(dns_json_query,json_string(query->source_name));
  return dns_json_query;
}

Query* jsonformat_to_query(json_t* json_format_dns_query)
{
  Query *query = ( Query*)malloc(sizeof(Query));
  query->Type= json_integer_value(json_array_get(json_format_dns_query, 1));
  query->source_name=(char*)json_string_value(json_array_get(json_format_dns_query, 2));
  return query;
}

json_t* reply_to_jsonformat(Reply *reply)
{
  json_t  *dns_json_query = json_array();
  int cnt = reply->current_count;
  json_array_append_new(dns_json_query,json_integer(CURRENT_VERSION));
  json_array_append_new(dns_json_query,json_integer(reply->Type));
  json_array_append_new(dns_json_query,json_integer(cnt)); // if current_count<N
  for(int i = 0; i<cnt ;i++)
  {
    json_array_append_new(dns_json_query,json_string(reply->mas_str[i])); // if current_count<N
  }
  return dns_json_query;
}


Reply* jsonformat_to_reply(json_t* json_format_dns_reply)
{
   Reply *reply = ( Reply*)malloc(sizeof(Reply));
   int Type = json_integer_value(json_array_get(json_format_dns_reply, 1));
   int N = json_integer_value(json_array_get(json_format_dns_reply, 2));
   reply_init(reply,N,Type);

   for(int i = 0; i<N ;i++)
   {
      reply_add_str(reply,(char*)json_string_value(json_array_get(json_format_dns_reply, 3+i)));
   }
   return reply;

}















void reply_deinit(Reply *reply)
{
  free(reply->mas_str);
}
