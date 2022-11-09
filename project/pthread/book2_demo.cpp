// 本程序讨论线程参数的传递, 这个案例是视频中没有写出的补充
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

/*
知识点：
1. 创建的多个线程并不能保证那个线程先运行
2. 不能用全局变量替代线程的参数
3. 数据类型的强制转换
*/



int var;
void *thmain1(void *arg);    // 线程1的主函数。
void *thmain2(void *arg);    // 线程2的主函数。
void *thmain3(void *arg);    // 线程3的主函数。
void *thmain4(void *arg);    // 线程4的主函数。
void *thmain5(void *arg);    // 线程5的主函数。

int main()
{

    pthread_t thid1=0,thid2=0,thid3=0,thid4=0,thid5=0;   // 线程id typedef unsigned long pthread_t

    // 创建线程。
    var=1;
    if (pthread_create(&thid1,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

    var=2;
    if (pthread_create(&thid2,NULL,thmain2,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

    var=3;
    if (pthread_create(&thid3,NULL,thmain3,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

    var=4;
    if (pthread_create(&thid4,NULL,thmain4,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

    var=5;
    if (pthread_create(&thid5,NULL,thmain5,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

    // 等待子线程退出。
    printf("join...\n");
    pthread_join(thid1,NULL);  
    pthread_join(thid2,NULL);  
    pthread_join(thid3,NULL);  
    pthread_join(thid4,NULL);  
    pthread_join(thid5,NULL);  
    printf("join ok.\n");
    return 0;
}

void *thmain1(void *arg)    // 线程主函数。
{
  printf("var1=%d\n",var);
  printf("线程1开始运行。\n");
}

void *thmain2(void *arg)    // 线程主函数。
{
  printf("var2=%d\n",var);
  printf("线程2开始运行。\n");
}

void *thmain3(void *arg)    // 线程主函数。
{
  printf("var3=%d\n",var);
  printf("线程3开始运行。\n");
}

void *thmain4(void *arg)    // 线程主函数。
{
  printf("var4=%d\n",var);
  printf("线程4开始运行。\n");
}

void *thmain5(void *arg)    // 线程主函数。
{
  printf("var5=%d\n",var);
  printf("线程5开始运行。\n");
}
