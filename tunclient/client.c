//
// gcc client.c -o client -ljansson
//
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

#include <jansson.h>//jansson.h>
#include <jansson_config.h>

#define MESSAGE_SIZE 2000
#define READ_FILE_BUF_SIZE 100


#define CURRENT_VERSION 1
#define NUMBER_FIELD 3

#define T_A 1
#define T_TXT 16


int dns_tun_client_init(char *server_ip,int port);
int find_char(char* str, int n, char s,int startpos);
char* extract_str(char* str);
json_t* create_dns_query(int version, int type_query, char* resourse_name);

 struct Client_source
{
  int file_descriprion;
  int sock_description;
}Client_source;

int dns_tun_client_init(char *server_ip,int port) // return a socket descriptoin
{
  int sock;
  struct sockaddr_in server;

  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1) {
      printf("Could not create socket");
      return -1;
  }
  puts("Socket created");

  server.sin_addr.s_addr = inet_addr(server_ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
      perror("connect failed. Error");
      return -1;
  }
  puts("Connected\n");
  return sock;
}




int  generate_dns_query(int sock,char *input_file,char *output_file)
{
  FILE *f_in = fopen(input_file,"r");
  FILE *f_out = fopen(output_file, "w");
  if(f_in==NULL ){  //  printf("File not found\\Error\n");
    return -1;
  }
  char file_line[READ_FILE_BUF_SIZE];
  char *mnemonic_name, *query_to_char;
  json_t* DNStoTCPquery;
  char buf_for_transfer[MESSAGE_SIZE];
  while(!feof(f_in))
  {
    if(fgets(file_line,100,f_in)!=NULL)
    {
      if((file_line[0]=='A')||((file_line[0]=='T')&&(file_line[1]=='X')&&(file_line[0]=='T')))
      {
        printf("%s",file_line);
        mnemonic_name =extract_str(file_line);
        if(file_line[0]=='A') { DNStoTCPquery = create_dns_query(CURRENT_VERSION,1,mnemonic_name);} // A-record
        else { DNStoTCPquery = create_dns_query(CURRENT_VERSION,16,mnemonic_name);}   // TXT-record
        // printf("==->!%s!\n", json_string_value(json_array_get(DNStoTCPquery, 2)));

//  ????????    for(int i=0;i<MESSAGE_SIZE;i++) buf_for_transfer[i]='\0';

         query_to_char = json_dumps(DNStoTCPquery,0);
         strcpy(buf_for_transfer,query_to_char);


         if( send(sock , buf_for_transfer , strlen(buf_for_transfer) , 0) < 0)
         {
             puts("Send failed");
             fclose(f_in);
             //close(sock);
             return 1;
         }
         char server_reply[2000];
         if( recv(sock , server_reply , 2000 , 0) < 0) {  puts("recv failed"); break; }

  //       json_t* newjson = json_loads(query_to_char,0,NULL);
  //       printf("==->?%s?\n\n\n", json_string_value(json_array_get(newjson, 2)));


      }
    }

  }

}



int main()
{
  struct Client_source *fssource = (struct Client_source*)calloc(1,sizeof(Client_source));



  int sock = dns_tun_client_init("127.0.0.1",8888);
  if(sock==-1) { return -1; }
  if(generate_dns_query(sock,"input","outup")==-1) { printf("File error\n"); return -1;}
  return 0;
  return -1;


  char message[MESSAGE_SIZE] , server_reply[MESSAGE_SIZE];

  while(1)
   {
       printf("Enter message : ");
       scanf("%s" , message);

       //Send some data
       if( send(sock , message , strlen(message) , 0) < 0)
       {
           puts("Send failed");
           return 1;
       }

       //Receive a reply from the server
       if( recv(sock , server_reply , 2000 , 0) < 0)
       {
           puts("recv failed");
           break;
       }

       puts("Server reply :");
       puts(server_reply);
       memset(server_reply,0,MESSAGE_SIZE);    //   for(int i=0;i<2000;i++){server_reply[i]='\0';}

   }
}






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
  //printf("\n");
  char* source = (char*)malloc((strlen(str)-1)*sizeof(char));
  memset(source,0,(strlen(str)-1)*sizeof(char));

  for(int i=0;i<m-k;i++)
  {
    source[i]=str[k+i];
  }
  return source;
}

json_t* create_dns_query(int version, int type_query, char* resource_name)
{
    json_t  *DNStoTCPquery = json_array();

    json_array_append_new(DNStoTCPquery,json_integer(version));
    json_array_append_new(DNStoTCPquery,json_integer(type_query));
    json_array_append_new(DNStoTCPquery,json_string(resource_name));
    return DNStoTCPquery;
}
