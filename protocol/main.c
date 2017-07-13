#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <jansson.h>//jansson.h>
#include <jansson_config.h>

#include "protocol.h"




int main()
{
   Reply *reply = ( Reply*)malloc(sizeof(Reply));
   reply_init(reply,3,T_TXT);
   printf("N = %d\t",reply->N);
   printf("Type = %d\t",reply->Type);

   reply_add_str(reply,"Hello");
   reply_add_str(reply,"my");
   reply_add_str(reply,"World");
   reply_add_str(reply,"no added"); // because N<current_count
   for(int i = 0;i<reply->current_count;i++)
   {
     printf("%s\t",reply->mas_str[i]);
   }
   printf("\n");
   json_t *temp=reply_to_jsonformat(reply);

   Reply *new_reply = jsonformat_to_reply(temp);

   printf("N = %d\t",new_reply->N);
   printf("Type = %d\t",new_reply->Type);

   for(int i = 0;i<new_reply->current_count;i++)
   {
     printf("%s\t",new_reply->mas_str[i]);
   }
   printf("\n\n\n");



}
