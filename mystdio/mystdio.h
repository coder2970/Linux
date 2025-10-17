#ifndef __MYSTDIO_H__
#define _MYSTDIO_H

#define FLUSH_NONE 1
#define FLUSH_LINE 2
#define FLUSH_FULL 4

#define FORCE 1
#define NORMAL 2

#define SIZE 4096
#define UMASK 0666

typedef struct _MY_IO_FILE
{
    int fileno;
    int flag;
    char outbuffer[SIZE];
    int curr;
    int cap;
}MYFILE;

MYFILE* myfopen(const char* filename,const char* mode);
int myfwrite(const char* s,int size,MYFILE* fp);
void myfclose(MYFILE* fp);
void myfflush(MYFILE* fp);

#endif