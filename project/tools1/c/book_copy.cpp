/*
共享内存的简单学习，并开发守护程序
作者：王梓伊
*/

// 注意共享内存在使用时需要上锁，需要使用信号量
#include"_public.h"

CSEM sem; // 用于给共享内存加锁的信号量

struct st_pid{
    int pid; // 进程编号
    char name[51]; // 进程名称

};                                                                             


int main(int argc, char *argv[])
{
    // 共享内存的标志
    int shmid;
    // 创建共享内存,键值为0x5005
    if( (shmid = shmget(0x5005, sizeof(struct st_pid), 0640|IPC_CREAT)) == -1)
    {
        printf("shmget(0x5005) failed\n");
        return -1;
    }

    // 如果信号量已经存在，获取信号量，如果信号量不存则创建并完成初始化
    if(sem.init(0x5005) == false)
    {printf("sem.init(0x5005) failed\n"); return -1;}

    // 用于指向共享内存的结构体变量
    struct st_pid *stpid = 0;

    // 把共享内存链接到当前进程的地址空间
    if( (stpid= (struct st_pid* )shmat(shmid,0,0)) == (void* )-1)
    {
        printf("shmget(0x5005) failed \n");
        return -1;
    }
    printf("aaa  time = %d, val = %d\n",time(0), sem.value());
    sem.P(); // 加锁
    printf("bbb  time = %d, val = %d\n",time(0), sem.value());
    printf("pid=%d, name=%s\n",stpid->pid, stpid->name);
    stpid->pid = getpid();
    sleep(10);
    strcpy(stpid->name, argv[1]);
    printf("pid=%d, name=%s\n",stpid->pid, stpid->name);
    printf("ccc  time = %d, val = %d\n",time(0), sem.value());
    sem.V(); // 解锁
    printf("ddd  time = %d, val = %d\n",time(0), sem.value());

    // 把共享内存从当前进程中分离
    shmdt(stpid);

    // 删除共享内存
    // if (shmctl(shmid, IPC_PMID, 0) == -1)
    // {printf("shmctl failed\n"); return -1;}
    return 0;
}