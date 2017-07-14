#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <jansson.h>
#include <jansson_config.h>

#include "protocol.h"




int main()
{

//       An example of work with the reply

   Reply *reply = ( Reply*)malloc(sizeof(Reply));
   reply_init(reply,3,T_TXT);

   reply_add_str(reply,"Hello");
   reply_add_str(reply,"my");
   reply_add_str(reply,"World");
   reply_add_str(reply,"no added"); // because N<current_count
   printf("\n");

   for(int i = 0;i< current_count_of_reply(reply);i++)   {     printf("%s\t",reply_pop_str(reply, i));   }
   printf("\n");

   printf("type_of_reply = %d\n",type_of_reply(reply));
   printf("max_count_of_reply = %d\n",max_count_of_reply(reply));
   printf("current_count_of_reply = %d\n",current_count_of_reply(reply));

   printf("\n\n\n");
   json_t *temp=reply_to_jsonformat(reply);

   Reply *new_reply = jsonformat_to_reply(temp);


   for(int i = 0;i< current_count_of_reply(new_reply);i++)      {     printf("%s\t",reply_pop_str(new_reply, i));   }
   printf("\n");
   printf("type_of_reply = %d\n",type_of_reply(new_reply));
   printf("max_count_of_reply = %d\n",max_count_of_reply(new_reply));
   printf("current_count_of_reply = %d\n",current_count_of_reply(new_reply));

   printf("\n\n\n");


//       An example of work with the query


   Query *query = ( Query*)malloc(sizeof(Query));
   query_init(query,T_TXT,"google.com");

   printf("%d\t",type_of_query(query));
   printf("%s\n",source_name_of_query(query));


   json_t* j_query=query_to_jsonformat(query);
   Query *new_query=jsonformat_to_query(j_query);


   printf("%d\t",type_of_query(new_query));
   printf("%s\n",source_name_of_query(new_query));


   reply_init(reply,3,T_TXT);


}
