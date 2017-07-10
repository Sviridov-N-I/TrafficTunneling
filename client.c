/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

#include </usr/include/jansson.h>//jansson.h>
#include <jansson_config.h>
//#include <rw.h>


#define CURRENT_VERSION 1
#define NUMBER_FIELD 3

#define T_A 1
#define T_TXT 16

#define MESSAGE_SIZE 2000

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
  printf("\n");
  char* soure = (char*)malloc((strlen(str)-1)*sizeof(char));
  memset(soure,0,(strlen(str)-1)*sizeof(char));

  for(int i=0;i<m-k;i++)
  {
    soure[i]=str[k+i];
  //  printf("%c_",soure[i]);
  }
/*  printf("\n\t\t\tIN EXTRACT: %s\n",soure);
  printf("\t\t\tsoure[k]=%c\n",str[k]);
  printf("\t\t\tsoure[k]=%c\n",soure[0]);
  printf("\t\t\tk=%d\n",k);
  printf("\t\t\tm=%d\n",m);
  printf("\t\t\tm-k=%d\n",m-k);*/
//  printf("soure[m-1]=%c\n"soure[k]);
  return soure;
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





int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[MESSAGE_SIZE] , server_reply[2000];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    json_t  *DNStoTCPquery;//= create_dns_query(1,T_TXT,"google.com");
  //  json_t  *DNStoTCPquery= create_dns_query(1,T_A,"google.com");


  //  char *s =read_dns_query_soure_name(DNStoTCPquery);
  //  if(s!=NULL) printf("Resoure name: \"%s\"\n\n\n\n", s);

    char* ToChar=NULL;
    if(ToChar!=NULL) printf("json_dumps() successful\n");


    int k=0; //delete!!!

    const char f_in_name[200]="text";
    const char f_out_name[180]="out";
    FILE *input_file,*outup_file;

    input_file = fopen(f_in_name,"r");
    outup_file = fopen(f_out_name,"w");

    if(input_file==NULL ){
      printf("File not found\\Error\n");
      close(sock);
      return -1;
    }
    if( outup_file==NULL){
      printf("Error\n");
      close(sock);
      return -1;
    }

    char str[100];
    char *soure;
    json_t* temp;

    while(!feof(input_file))
    {
      if(fgets(str,100,input_file)!=NULL)
      {
        if((str[0]=='A')||((str[0]=='T')&&(str[1]=='X')&&(str[0]=='T')))
        {
          soure = extract_str(str);
          printf("\n\nsoure=%s\n",soure);
          if(str[0]=='A')
          {DNStoTCPquery = create_dns_query(CURRENT_VERSION,1,soure);} // A-record
          else {DNStoTCPquery = create_dns_query(CURRENT_VERSION,16,soure);}   // TXT-record
        //  printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(DNStoTCPquery, 0)));
        //  printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(DNStoTCPquery, 1)));
          printf(" json==->%s\n", json_string_value(json_array_get(DNStoTCPquery, 2)));
          printf("\n");
        }
      }





      ToChar=json_dumps(DNStoTCPquery,0);

    //    printf("Enter message : ");
    //    scanf("%s" , message);
    //    strcpy(message,"");
        strcpy(message,ToChar);
      //  printf("\nSENDsize=%d\n",json_array_size(DNStoTCPquery));
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            fclose(input_file);
            close(sock);
            return 1;
        }

        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
    //    printf("k= %d\n",k);
    //    puts("Server reply :");
  //      puts(server_reply);

        json_error_t error;
        json_t* newjson = NULL;
        newjson = json_loads(server_reply,0,&error);
    //    if(newjson!=NULL)  printf("YES_NEW\n");
    //    printf("\nREPLYsize=%d\n",json_array_size(newjson));
      //  fputs((char*)(json_integer_value(json_array_get(newjson, 0))+'0'),outup_file);
      //fputs("\t",outup_file);
        int N=(int)json_integer_value(json_array_get(newjson, 2));
        printf("N=%d\n",N);
        printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(newjson, 0)));
        printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(newjson, 1)));
        printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(newjson, 2)));
        for(int i=0 ; i < json_integer_value(json_array_get(newjson, 2))  ; i++)
        {

          if(json_integer_value(json_array_get(newjson, 1))==T_A)  fputs("A, ",outup_file);
          if(json_integer_value(json_array_get(newjson, 1))==T_TXT)  fputs("TXT, ",outup_file);
          fputs(json_string_value(json_array_get(DNStoTCPquery, 2)),outup_file);
          fputs(" ",outup_file);
          /*if(N==0)
          {
            printf("------------------------>\n");
            printf("==->%" JSON_INTEGER_FORMAT "\n", json_integer_value(json_array_get(newjson, 2)));
            fputs("infromation about this soure not found\n",outup_file);
            printf("infromation about this soure not found\n");
            continue;
          }*/

          printf("==->%s\n", json_string_value(json_array_get(newjson, 3+i)));
          fputs(json_string_value(json_array_get(newjson, 3+i)),outup_file);
          fputs("\n",outup_file);

          for(int i=0;i<MESSAGE_SIZE;i++)
                    message[i]='\0';
        //  memset(message,0,strlen(message));
        }




    //    printf("==->%s\n", json_string_value(error));


    }
    fclose(input_file);
    close(sock);
    return 0;
}
