#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>


typedef struct Reply
{
   int Type;
   int N; // all number of record
   int current_count;
   char **mas_str;
}Reply; //  struct Client_source *fssource = (struct Client_source*)calloc(1,sizeof(Client_source));

typedef struct Query
{
   int Type;
   char *source_name;
}Query;


void reply_init( Reply *reply, int N,int type_query);
int reply_add_str(Reply *reply, char *str);
void reply_deinit(Reply *reply);

void query_init(Query *query, int type_query,char *source_name);

void query_init(Query *query, int type_query,char *source_name)
{
  query->Type = type_query;
  query->source_name = source_name;
}


int main()
{
   Reply *reply = ( Reply*)malloc(sizeof(Reply));
   reply_init(reply,4,1);
   printf("current_count = %d\n",reply->current_count);
   printf("N = %d\n",reply->N);
   printf("Type = %d\n",reply->Type);

   reply_add_str(reply,"Hello");
   reply_add_str(reply,"my");
   reply_add_str(reply,"World");
   reply_add_str(reply,"no added");
   for(int i = 0;i<reply->current_count;i++)
   {
     printf("%s\n",reply->mas_str[i]);
   }

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


void reply_deinit(Reply *reply)
{
  free(reply->mas_str);
}
