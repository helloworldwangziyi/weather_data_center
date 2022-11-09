#include "_public.h"

void EXIT(int sig)
{
    printf("sig = %d", sig);
    if(sig == 2) exit(0);
    
}
// exit 函数不会调用局部对象的析构函数
// exit 函数会调用全局对象的析构函数
// return 会调用局部和全局对象的析构函数


CPActive Active;
int main(int argc, char * argv[])
{
    if(argc != 3) 
    {
        printf("Using :./book2 procname timeout\n");
        return 0;
    }

    // 设置2和15的信号
    signal(2, EXIT);
    signal(15, EXIT);

   
    Active.AddPInfo(atoi(argv[2]), argv[1]);

    while(1)
    {
        // Active.UptATime();
        sleep(10);
    }
    return 0;

}