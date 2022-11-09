// 本程序演示线程的创建和终止

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

int var = 0;

// 线程主函数
void * thread_func(void* arg);
void * thread_func2(void * arg);

int main()
{
    pthread_t pthread1 = 0;
    pthread_t pthread2 = 0;


    // 创建线程
    if(pthread_create(&pthread1, NULL, thread_func, NULL)!=0) 
    {
        printf("pthread_create(thread_func) false");
        exit(-1);
    }
    if(pthread_create(&pthread2,NULL,thread_func2, NULL)!=0)
    {
        printf("pthread_create(thread_func2) false");
        exit(-1);
    }

    // 等待线程退出
    printf("join ...");
    pthread_join(pthread1, NULL);
    pthread_join(pthread2, NULL);
    printf("join ok");


}
// return 只能从函数回到线程但是无法终止线程
void func1()
{
    return;
}

void *thread_func(void *arg)    // 线程主函数。
{
  for (int ii=0;ii<5;ii++)
  {
    var=ii+1;
    sleep(1); printf("thread_func sleep(%d) ok.\n",var);
    if (ii==2) func1();
  }
}

void func2()
{
    pthread_exit(0);
}

void * thread_func2(void *arg)
{
    for(int ii = 0; ii < 20; ii++)
    {
        var = ii + 1;
        sleep(1);
        printf("thread_func2 sleep(%d) ok \n", var);
        if( ii == 2) func2();
    }

}