// 多线程版服务端程序

#include "../_public.h"

// 创建日志对象和socket对象
CLogFile logfile;

CTcpServer server;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;      // 声明互斥锁。

vector<pthread_t>vthid; // 申明线程容器

void *thmain(void *arg);       // 线程主函数。（用于处理客户端发来的数据）

void thcleanup(void *arg);     // 线程清理函数。

void EXIT(int sig);    // 进程的退出函数。

// 主线程负责监听，而子线程负责进行通信
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("参数过少，请按照案例输入参数\n");
        printf("输入示例：./demo20_sock 5005 /tmp/demo20_sock.log\n");
        return -1;
    }
    // 关闭全部的信号和输入输出。
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
    // 但请不要用 "kill -9 +进程号" 强行终止
    // CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
    // 如果打开日志文件失败就返回
    if(logfile.Open(argv[2], "a+") == false)
    {
        printf("logfile.Open(%s) failed", argv[2]);
        return -1;
    }
    // 初始化服务端
    if(server.InitServer(atoi(argv[1]))==false) 
    {
        logfile.Write("server.InitServer(%s) failed", argv[1]);
        return -1;
    }
    while(1)
    {
        // 等待客户端的连接请求
        if(server.Accept() == false)
        {
            logfile.Write("server.Accept() failed");
            EXIT(-1); 
        }
        logfile.Write("客户端已连接(%s)\n",server.GetIP());

        // 创建一个新的线程来
        pthread_t thid;

        // 把客户端连接上来的socket 传给线程处理函数,创建失败就关闭连接
        if(pthread_create(&thid, NULL, thmain, (void*)(long)server.m_connfd)!=0)
        {
            logfile.Write("pthread_create() failed.\n"); server.CloseListen(); continue;
        }

        // 将线程id放入容器
        pthread_mutex_lock(&mutex);    // 加锁。
        vthid.push_back(pthread_self());
        pthread_mutex_unlock(&mutex);  // 解锁
    }
}

void *thmain(void *arg)      // 线程主函数。（用于处理客户端发来的数据）
{
    pthread_cleanup_push(thcleanup,arg);       // 把线程清理函数入栈（关闭客户端的socket）。
    //  客户端的socket
    int connfd = (int)(long)(arg);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);   // 线程取消方式为立即取消。

    pthread_detach(pthread_self());           // 把线程分离出去。

    // 子线程与客户端进行通讯，处理业务。
    int  ibuflen;
    char buffer[102400];

    // 与客户端通讯，接收客户端发过来的报文后，回复ok。
    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        if (TcpRead(connfd,buffer,&ibuflen,30)==false) break; // 接收客户端的请求报文。
        logfile.Write("接收：%s\n",buffer);

        strcpy(buffer,"ok");
        if (TcpWrite(connfd,buffer)==false) break; // 向客户端发送响应结果。
        logfile.Write("发送：%s\n",buffer);
    }
    // 关闭连接
    close(connfd);

    pthread_mutex_lock(&mutex);
    for (int ii=0;ii<vthid.size();ii++)
    {
        if (pthread_equal(pthread_self(),vthid[ii])) { vthid.erase(vthid.begin()+ii); break; }
    }
    pthread_mutex_unlock(&mutex);

    pthread_cleanup_pop(1);         // 把线程清理函数出栈。
}

// 进程的退出函数。
void EXIT(int sig)  
{
  // 以下代码是为了防止信号处理函数在执行的过程中被信号中断。
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  logfile.Write("进程退出，sig=%d。\n",sig);

  server.CloseListen();    // 关闭监听的socket。

  // 取消全部的线程。
  for (int ii=0;ii<vthid.size();ii++)
  {
    pthread_cancel(vthid[ii]);
  }

  sleep(1);        // 让子线程有足够的时间退出。

  pthread_mutex_destroy(&mutex);

  
  exit(0);
}

void thcleanup(void *arg)     // 线程清理函数。
{
  close((int)(long)arg);      // 关闭客户端的socket。

  logfile.Write("线程%lu退出。\n",pthread_self());
}