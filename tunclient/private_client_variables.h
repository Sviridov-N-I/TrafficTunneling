#ifndef PRIVATE_VARIABLES_H_
#define PRIVATE_VARIABLES_H_

#include <stdio.h>
#define MESSAGE_SIZE 2000
#define READ_FILE_BUF_SIZE 500

#define T_A 1
#define T_TXT 16


typedef struct Client_resource
{
  int sock;
  char *input_file;
  char *output_file;
  FILE *input_file_descr;
  FILE *output_file_descr;
}Client_resource;


#endif /*#ifndef PRIVATE_VARIABLES_H_*/
