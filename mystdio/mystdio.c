#include "mystdio.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

MYFILE* myfopen(const char* filename,const char* mode)
{
    int fd = -1;
    if(strcmp(mode,"w") == 0)
    {
        fd = open(filename,O_CREAT | O_WRONLY | O_TRUNC,UMASK);
    }
    else if(strcmp(mode,"r") == 0)
    {
        fd = open(filename,O_RDONLY);
    }
    else if(strcmp(mode,"a") == 0)
    {
        fd = open(filename,O_CREAT | O_WRONLY | O_APPEND, UMASK);
    }
    if(fd < 0)
    return NULL;

    // 打开成功
    MYFILE* fp = (MYFILE*)malloc(sizeof(MYFILE));
    if(fp == NULL)
    return NULL;

    fp->fileno = fd;
    fp->flag = FLUSH_LINE;
    fp->curr = 0;
    fp->cap = SIZE;
    fp->outbuffer[0] = 0;

    return fp;
}   
static void myfflush_core(MYFILE* fp,int force)
{
    if(fp->curr < 0) return;
    if(force == FORCE)
    {
        write(fp->fileno,fp->outbuffer,fp->curr);
        fp->curr = 0;
    }
    else if(force == NORMAL)
    {
        if((fp->flag & FLUSH_LINE) && (fp->outbuffer[fp->curr - 1] =='\n' ))
        {
            write(fp->fileno,fp->outbuffer,fp->curr);
            fp->curr = 0;
        }
        else if((fp->flag & FLUSH_FULL) && (fp->curr == fp->cap))
        {
            write(fp->fileno,fp->outbuffer,fp->curr);
            fp->curr = 0;
        }
        else
        {}

    }
}
void myfflush(MYFILE* fp)
{
    myfflush_core(fp,NORMAL);
}


int myfwrite(const char* s,int size,MYFILE* fp)
{
    memcpy(fp->outbuffer + fp->curr,s,size);
    // 更新curr
    fp->curr += size;
    myfflush_core(fp,NORMAL);

    return size;
}

void myfclose(MYFILE* fp)
{
    if(fp->fileno >= 0)
    {
        myfflush(fp);
        fsync(fp->fileno);
        close(fp->fileno);
        free(fp);
    }
}