/*
共享内存的简单学习，并开发守护程序
作者：王梓伊
*/

// 注意共享内存在使用时需要上锁，需要使用信号量
#include"_public.h"

#define MAXNUMP_ 1000 // 最大的进程数量
#define SHMKEYP_ 0x5095 // 共享内存的key

struct st_pinfo
{
    int pid;        // 进程id
    char pname[51]; // 进程名称，可以为空
    int timeout;    // 超时时间 单位 秒
    time_t atime;   // 最后一次心跳的时间，用整数表示
};

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        printf("Using :./book procname\n");
        return 0;
    }

    CSEM m_sem;  // 创建信号量
    if( (m_sem.init(SHMKEYP_) == false))
    {
        printf("shmget(%x) failed\n",SHMKEYP_); return -1;
    }

    // 创建 获取共享内存，大小为n*sizeof(struct st_pinfo)
    int m_shmid = 0;
    // 参数代表的含义是键值， 共享内存大小， 共享内存权限以及在没有时创建
    if((m_shmid = shmget(SHMKEYP_, MAXNUMP_ * sizeof(struct st_pinfo), 0640|IPC_CREAT)) == -1)
    {
        printf("m_sem.init(%x) failed\n"); return -1;
    }

    // 将共享内存链接到进程的地址空间
    struct st_pinfo *m_shm;
    m_shm = (struct st_pinfo*)shmat(m_shmid, 0, 0);
    
    // 创建当前进程的心跳信息结构体变量，把本进程的信息填进去
    struct st_pinfo stpinfo;
    memset(&stpinfo, 0 ,sizeof(struct st_pinfo));
    stpinfo.pid = getpid();   // 进程id
    STRNCPY(stpinfo.pname, sizeof(stpinfo.pname), argv[1], 50);  // 进程名称
    stpinfo.timeout = 30; // 超时时间 单位秒
    stpinfo.atime = time(0);    // 最后一次心跳时间，填充当前时间   

    
    // 进程id 是循环使用的，如果曾经有一个进程异常退出，没有清理自己的心跳信息
    // 它的进程信息将残留在共享内存中，而不巧的是，当前进程重用了上述进程的id
    // 这样就会在共享内存中存在两个进程id相同的记录，守护进程检查到残留进程的心跳时
    // 会向进程id发送退出信号，这个信号将误杀当前进程
    int m_pos = -1;
    // 如果共享内存中存在当前进程编号，一定时其他进程残留的数据，当前进程就重用该位置
    for(int ii = 0; ii< MAXNUMP_;ii++)
    {
        if (m_shm[ii].pid == stpinfo.pid) {m_pos = ii; break;}
    }

    m_sem.P(); // 加锁
    // 在共享内存中查找一个空位置，把当前进程的心跳信息存入共享内存中
    if (m_pos == -1)
    {
        for (int ii = 0; ii < MAXNUMP_; ii++)
        {
            if((m_shm[ii].pid == 0))
            {
                // 找到一个空位置
                m_pos = ii;
                break;
            }
        }
    }
    
    if(m_pos == -1)
    {
        m_sem.V(); // 解锁
        printf("共享内存空间已经用完\n"); return -1;
    }
    // 把当前进程的心跳信息存入到内存的进程组中
    memcpy(m_shm + m_pos , &stpinfo, sizeof(struct st_pinfo));

    m_sem.V(); // 解锁
    while(true)
    {
        // 更新共享内存中本进程的心跳时间
        m_shm[m_pos].atime = time(0);
        sleep(10);
        
    }
    //  把当前进程从共享内存中移去
    // m_shm[m_pos].pid = 0;
    memset(m_shm+m_pos, 0, sizeof(struct st_pinfo));


    // 把共享内存从当前进程中分离
    shmdt(m_shm);

    return 0;
}
