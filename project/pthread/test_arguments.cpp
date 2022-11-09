// 本程序演示类型的强制转换
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int main()
{
    int ii = 10;
    void *pv =0;            // 指针占内存8个字节，用于存放变量的地址
    pv = (void *)(long)ii; // 先把ii转换成long，再转换成void*
    printf("pv = %p\n", pv); // pv存放的不是变量的地址，而是变量ii的值

    int jj = 0;
    jj = (long)pv;   // 把pv存放的内容先转换为long， 再转换为int
    printf("jj=%ld\n", jj);

    return 0;
}
