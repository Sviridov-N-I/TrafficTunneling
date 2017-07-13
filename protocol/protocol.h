#ifndef PROTOCOL_H_
#define PROTOCOL_H_

 #include "privat_struct.h"


#define CURRENT_VERSION 1

#define T_A 1
#define T_TXT 777

/*
typedef struct Query
{
   int Type;
   char *source_name;
}Query;

typedef struct Reply
{
   int Type;
   int N; // all number of record
   int current_count;
   char **mas_str;
}Reply;*/






void query_init(Query *query, int type_query,char *source_name);

void reply_init(Reply *reply, int N,int type_query);
int reply_add_str(Reply *reply, char *str);
void reply_deinit(Reply *reply);


json_t* query_to_jsonformat(Query *query);
Query* jsonformat_to_query(json_t* json_format_dns_query);

json_t* reply_to_jsonformat(Reply *reply);
Reply* jsonformat_to_reply(json_t* json_format_dns_reply);

//typedef struct Reply

//void rep1ly_init(reply_t *reply, int N,int type_query);


#endif /* PROTOCOL_H_*/
