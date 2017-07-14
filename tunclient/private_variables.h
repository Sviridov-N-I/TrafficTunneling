#ifndef PRIVATE_VARIABLES_H_
#define PRIVATE_VARIABLES_H_


#define MESSAGE_SIZE 2000
#define READ_FILE_BUF_SIZE 500


#define CURRENT_VERSION 1
#define NUMBER_FIELD 3

#define T_A 1
#define T_TXT 16

struct Client_source
{
 int file_descriprion;
 int sock_description;
}Client_source; //  struct Client_source *fssource = (struct Client_source*)calloc(1,sizeof(Client_source));


#endif /*#ifndef PRIVATE_VARIABLES_H_*/
