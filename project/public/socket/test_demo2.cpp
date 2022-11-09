#include"../_public.h"
using namespace std;

CFile File;

struct st_message
{
    // 传递给线程的文件名
    char message[300]; 
};

vector<struct st_message>vcache; // 用vector容器做缓存
vector<int>res; // 最终用于统计的容器，每个容器中装入的是不同线程的统计结果

// 声明并初始化条件变量和锁
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void incache(int sig); // 生产者，数据入队
void *outcache(void *arg); // 消费者，数据出队线程的主函数
void  thcleanup(void *arg); // 清理线程的函数

int main()
{
    signal(15, incache); // 接收15的信号调用生产者函数

    // 创建5个消费者线程
    pthread_t thid1, thid2, thid3,thid4,thid5;
    pthread_create(&thid1, NULL, outcache, NULL);
    pthread_create(&thid2, NULL, outcache, NULL);
    pthread_create(&thid3, NULL, outcache, NULL);
    pthread_create(&thid4, NULL, outcache, NULL);
    pthread_create(&thid5, NULL, outcache, NULL);

    pthread_join(thid1,NULL);
    pthread_join(thid2,NULL);
    pthread_join(thid3,NULL);
    pthread_join(thid4,NULL);
    pthread_join(thid5,NULL);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    

    for(int i =0; i<5;i++)
    {
        cout << res[i] << endl;
    }
    return 0;
}

void incache(int sig)       // 生产者、数据入队。
{
    CDir Dir;

    if(Dir.OpenDir("/project/public/socket","word_*txt")==false)
    {
        printf("打开目录失败\n");
    }
    printf("成功打开目录\n");

    // 拿到要处理文件列表
    pthread_mutex_lock(&mutex);    // 给缓存队列加锁。
    for(int i = 0; i<Dir.m_vFileName.size(); i++)
    {
        struct st_message stmesg;      // 消息内容。
        memset(&stmesg,0,sizeof(struct st_message));

        
        //  生产数据，放入缓存队列。
        strcpy(stmesg.message, Dir.m_vFileName[i].c_str());
        
        vcache.push_back(stmesg);
       
    }
    pthread_mutex_unlock(&mutex);  // 给缓存队列解锁。
    pthread_cond_broadcast(&cond); // 发送条件信号，激活全部的线程。
}

void  thcleanup(void *arg)
{
  // 在这里释放关闭文件、断开网络连接、回滚数据库事务、释放锁等等。
  printf("cleanup ok.\n");

  pthread_mutex_unlock(&mutex);
}

void *outcache(void *arg)    // 消费者、数据出队线程的主函数。
{
    pthread_cleanup_push(thcleanup,NULL);  // 把线程清理函数入栈。

    struct st_message stmesg;  // 用于存放出队的消息。

    while (true)
    {
        pthread_mutex_lock(&mutex);  // 给缓存队列加锁。

        // 如果缓存队列为空，等待，用while防止条件变量虚假唤醒。
        while (vcache.size()==0)
        {
            pthread_cond_wait(&cond,&mutex);
        }

        // 从缓存队列中获取第一条记录，然后删除该记录。
        memcpy(&stmesg,&vcache[0],sizeof(struct st_message)); // 内存拷贝。
        vcache.erase(vcache.begin());

        pthread_mutex_unlock(&mutex);  // 给缓存队列解锁。

        // 以下是处理业务的代码。
        // 创建CFile对象读取文件中的数据，并求和。将结果插入到总统计容器中
        CFile File;
        char buffer[300];
        
        if(File.Open(stmesg.message,"r") == false)
        {
            printf("打开文件失败\n");
        }
        printf("打开文件成功\n");
        File.Fgets(buffer,10);
        int result = atoi(buffer);
        
        pthread_mutex_lock(&mutex);
        res.push_back(result);
        pthread_mutex_unlock(&mutex);
        break;
    }

    pthread_cleanup_pop(1);  // 把线程清理函数出栈。
}