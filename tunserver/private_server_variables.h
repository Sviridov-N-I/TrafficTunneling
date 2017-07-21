#ifndef PRIVATE_SERVER_VARIABLES_H_
#define PRIVATE_SERVER_VARIABLES_H_


#define DNS_SERVER_IP "8.8.8.8"
#define BUFSIZE 65536


struct DNS_HEADER
{
    unsigned short id;

    unsigned char rd :1;
    unsigned char tc :1;
    unsigned char aa :1;
    unsigned char opcode :4;
    unsigned char qr :1;

    unsigned char rcode :4;
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1;
    unsigned char ra :1;

    unsigned short q_count:16;
    unsigned short ans_count:16;
    unsigned short auth_count:16;
    unsigned short add_count:16;
};

struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

#pragma pack(push, 2)
struct R_DATA
{
    unsigned short type:16;
    unsigned short _class:16;
    unsigned int ttl:32;
    unsigned short data_len:16;
};
#pragma pack(pop)

struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;


#endif /*#ifndef PRIVATE_SERVER_VARIABLES_H_*/