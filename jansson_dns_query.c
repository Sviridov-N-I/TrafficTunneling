
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/jansson.h>//jansson.h>
#include <string.h>
#include <jansson_config.h>


#define CURRENT_VERSION 1
#define NUMBER_FIELD 3


char* read_dns_query_soure_name(json_t *query);
json_t* create_dns_query(int version,int type_query, char* resourse_name);




int main(int argc, char *argv[])
{

  //  printf("JANSSON_VERSION = %s\n",JANSSON_VERSION);


    json_t  *DNStoTCPquery= create_dns_query(1,2,"google.ru");



    char *s =read_dns_query_soure_name(DNStoTCPquery);
    if(s!=NULL) printf("Resoure name: \"%s\"\n", s);

    return 0;
}


char* read_dns_query_soure_name(json_t *query)
{
    size_t size = json_array_size(query);
    if(size != NUMBER_FIELD) // структура запроса содержит 3 поля, ПРОВЕРИТЬ
    {
      printf("\t The packege structure was broken\n");
      return NULL;
    }

    json_t *query_type = json_array_get(query, 0);
    if(json_typeof(query_type)!=JSON_INTEGER){ // если на второй позиции не число, то нет смысла его проверять
      printf("\t The packege structure was broken\n");
      return NULL;
    }
    else{
      if(json_integer_value(query_type)!=CURRENT_VERSION) {  printf("\tVersions aren't equal\n"); return NULL; }// проверка совместимости версий
//      else { printf("Versions are equal\n"); }
    }

    return (char*)json_string_value(json_array_get(query, 2));
}

json_t* create_dns_query(int version, int type_query, char* resourse_name)
{
    json_t  *DNStoTCPquery = json_array();

    json_array_append_new(DNStoTCPquery,json_integer(version));
    json_array_append_new(DNStoTCPquery,json_integer(type_query));
    json_array_append_new(DNStoTCPquery,json_string(resourse_name));
    return DNStoTCPquery;
}
