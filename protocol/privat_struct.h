#ifndef PRIVATE_STRUCT_H_
#define PRIVATE_STRUCT_H_




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



#endif /* PRIVATE_STRUCT_H_ */
