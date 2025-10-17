#include "mystdio.h"
#include <string.h>
#include <unistd.h>

int main()
{
    MYFILE* fp = myfopen("log.txt","a");
    if(fp == NULL)
    return 1;
    
    const char* s = "HELLO WORLD";
    int cnt = 10;
    while(cnt --)
    {
        myfwrite(s,strlen(s),fp);
        sleep(1);
    }
    // myfclose(fp);
    return 0;
}