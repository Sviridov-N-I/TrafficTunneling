#ifndef PRIVATE_STRUCTURES_H_
#define PRIVATE_STRUCTURES_H_


#define CURRENT_VERSION 1

#define T_A 1
#define T_TXT 16

typedef struct Query
{
   int Type;
   char *source_name;
}Query;

typedef struct Reply
{
   int Type;
   int N; // all number of record
   int current_count;
   char **mas_str;
}Reply;



#endif /* PRIVATE_STRUCTURES_H_ */
