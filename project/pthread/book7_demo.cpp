#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thmain(void *arg)
{
    pthread_detach(pthread_self());
    for (int ii=0;ii<3;ii++)
    {
        sleep(1); printf("pthmain sleep(%d) ok.\n",ii+1);
    }
    return (void *) 1;
}

int main()
{
    pthread_t thid;

    if(pthread_create(&thid,NULL,thmain,NULL)!= 0)
    {
        printf("pthread_create failed");
        exit(-1);
    }

    sleep(5);

    int result = 0;
    void *ret;
    printf("join ...");
    result = pthread_tryjoin_np(thid,NULL);

    printf("result = %d", result);
}