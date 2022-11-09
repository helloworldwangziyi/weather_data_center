/*
本程序是实现生成全国气象站点分钟观测数据的调度程序
作者： 王梓伊
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>


int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Using:./procctl timetvl program argv ...\n");
        printf("Example:./project/tools1/procctl 5  program /usr/bin/ls -lt /tmp \n\n");

        printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本\n");
        printf("program  被调用的程序名，必须使用全路径.\n");
        printf("argv 被调度程序的的参数 \n");
        printf("注意 本程序不会被kill杀死，但可以用kill -9 强行杀死 \n\n\n");
        return -1;
    }

    // 先执行fork函数，创建一个子进程，让子进程，调用execl执行新的程序
    // 新的程序将替换子进程，不会影响父进程
    // 在父进程中，可以调用wait 函数等待新程序运行的结果，这样就可以实现调度的功能
    // /project/tools1/bin/procctl 60 /project/idc1/bin/crtsurfdata5 /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata5.log xml,json,csv
    

    // 关闭信号和io，本程序不希望被打扰
    for(int ii = 0; ii < 64; ii++){
        signal(ii, SIG_IGN);
        close(ii);
    }

    // 创建子进程失败就退出
    if( fork()!= 0 ) exit(0);
    
    // 启用SIGCHLD信号，让父进程可以wait子进程退出的状态
    signal(SIGCHLD, SIG_DFL);


    char *pargv[argc];
    for(int ii = 2; ii < argc; ii++)
    {
        pargv[ii-2] = argv[ii];
    }
    pargv[argc - 2] = NULL;

    while(true)
    {
        // 子进程执行新的程序
        if( fork() == 0)
        {
            execv(argv[2], pargv);
            // 程序执行失败就退出，否则将会一直fork导致程序崩溃
            exit(0);
        }
        // 父进程在这里等待
        else{ 
            int status;
            wait(&status);
            sleep(atoi(argv[1]));
        }
    }


    // execl是用参数中指定的程序替换了当前进程的正文段、数据段、堆和栈
    return 0;
}