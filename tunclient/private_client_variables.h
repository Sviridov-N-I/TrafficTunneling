#ifndef PRIVATE_VARIABLES_H_
#define PRIVATE_VARIABLES_H_

#include <stdio.h>
#define MESSAGE_SIZE 2000
#define READ_FILE_BUF_SIZE 500

#define T_A 1
#define T_TXT 16

#define N_THREADS 5


typedef struct Client_resource
{
  int sock;
  struct sockaddr_in server;
  int *mas_sock;
  char *input_file;
  char *output_file;
  FILE *input_file_descr;
  FILE *output_file_descr;
}Client_resource;

typedef struct Thread_resource
{
  int sock;
  struct sockaddr_in server;
  FILE *input_file_descr;
  FILE *output_file_descr;
}Thread_resource;


#endif /*#ifndef PRIVATE_VARIABLES_H_*/
