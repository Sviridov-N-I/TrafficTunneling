#include <stdlib.h>
#include <stdio.h>

#include <jansson.h>//jansson.h>
#include <string.h>
#include <jansson_config.h>

#define CURRENT_VERSION 1

int find_char(char* str, int n, char s,int startpos);
char* extract_str(char* str);
json_t* create_dns_query(int version,int type_query, char* resourse_name);

/*void f(FILE *file)
{


}*/
/*
int main()
{
  const char fname[40]="text";
  FILE* file;
  file = fopen(fname,"r");
  if(file==NULL)
  {
    printf("File not found\n");
    return -1;
  }
  char str[100];
  char *soure;
  json_t* temp;
  while (!feof(file))
  {
    if(fgets(str,100,file)!=NULL)
    {
      if((str[0]=='A')||((str[0]=='T')&&(str[1]=='X')&&(str[0]=='T')))
      {
        soure = extract_str(str);
      //  printf("soure=%s\n",soure);
        if(str[0]=='A') temp = create_dns_query(CURRENT_VERSION,1,soure);
        else temp = create_dns_query(CURRENT_VERSION,16,soure);
        printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(temp, 0)));
        printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(temp, 1)));
        printf("==->%s\n", json_string_value(json_array_get(temp, 2)));
        printf("\n");
        free(temp);
      }
    }
  }

  fclose(file);

  return 0;
}
*//*
int find_char(char* str, int n, char s,int startpos)
{
  for(int i = startpos; i<n ; i++)
  {
    if(str[i]==s) return i;
    if(str[i]=='\n') return -1;
  }
  return -1;
}

char* extract_str(char* str)
{
  int k = find_char(str,strlen(str),' ',0)+1;
  int m = strlen(str)-1; // strlen(str)-2 - last simbol string

  char* soure = (char*)malloc((strlen(str)-1)*sizeof(char));
  for(int i=0;i<m-k;i++)
  {
    soure[i]=str[k+i];
  }
  return soure;
}
*/
json_t* create_dns_query(int version, int type_query, char* resourse_name)
{
    json_t  *DNStoTCPquery = json_array();

    json_array_append_new(DNStoTCPquery,json_integer(version));
    json_array_append_new(DNStoTCPquery,json_integer(type_query));
    json_array_append_new(DNStoTCPquery,json_string(resourse_name));
    return DNStoTCPquery;
}
