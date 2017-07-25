#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "private_structures.h"

//#include "protocol.c"
#include <jansson.h>
#include <jansson_config.h>




void query_init(Query *query, int type_query,char *source_name);

void reply_init(Reply *reply, int N,int type_query);

int reply_add_str(Reply *reply, char *str);

char* reply_pop_str(Reply *reply,int i); // return one str in reply structure

//////////////// new //////////////////////////////
int type_of_query(Query *query);
int type_of_reply(Reply *reply);
char* source_name_of_query(Query *query);
int max_count_of_reply(Reply *reply);
int current_count_of_reply(Reply *reply);
///////////////////////////////////////////////////


void reply_deinit(Reply *reply);


json_t* query_to_jsonformat(Query *query);

Query* jsonformat_to_query(json_t* json_format_dns_query);

json_t* reply_to_jsonformat(Reply *reply);

Reply* jsonformat_to_reply(json_t* json_format_dns_reply);




#endif /* PROTOCOL_H_*/
