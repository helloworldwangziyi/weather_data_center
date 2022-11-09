/*
 obcodetodb.cpp 本程序用于把全国站点分钟观测数据保存到数据库T_ZHUOBTCODE表中，支持xml和csv两种文件格式
 作者：王梓伊
*/
#include "_public.h"
#include "_mysql.h"

CLogFile logfile;

connection conn;

CPActive PActive;

void EXIT(int sig); // 信号处理函数

// 处理业务的主函数
bool _obtmindtodb(char *pathname, char *connstr, char *charset);

int main(int argc, char *argv[])
{
    // 帮助文档
    if(argc != 5)
    {
        printf("\n");
        printf("Using :./obtmindtodb pathfile connstr charset logfile\n");
        printf("Example:./project/tools1/bin/procctl 10 /project/idc1/bin/obtmindtodb /idcdata/surfdata \"127.0.0.1,root,Wangwang123456,mysql,3306\" utf8 /log/idc/obtmindtodb.log\n\n ");
        printf("本程序用于把全国站点分钟观测数据保存到数据库表中，数据只插入不更新\n");
        printf("pathfile 站点参数文件名（全路径）\n");
        printf("connstr 数据库链接参数: ip,username,password,dbname,port\n");
        printf("charset 数据库字符集 \n");
        printf("logfile 本程序运行的日志文件名\n");
        printf("程序每10秒运行一次，由procctil 调度 \n\n\n");
        
        return -1;

    }

    // 处理程序退出信号
    // 关闭全部信号和输入输出
    // 设置信号, 在shell状态下可用"kill + 进程号" 正常终止这些进程
    // 但请不要使用 kill -9 + 进程号 强行终止
    CloseIOAndSignal();
    signal(SIGINT,EXIT);
    signal(SIGTERM, EXIT);

    // 打开日志文件
    if (logfile.Open(argv[4],"a+")==false)
    {
        printf("打开日志文件失败（%s）。\n",argv[4]); return -1;
    }

    PActive.AddPInfo(30,"obtmindtodb"); // 进程的心跳，30秒足够

    // 处理业务的主函数
    _obtmindtodb(argv[1], argv[2], argv[3]);

    // // 链接数据库
    // if(conn.connecttodb(argv[2], argv[3]) != 0)
    // {
    //     logfile.Write("connect database(%s) failed.\n%s\n",argv[2], conn.m_cda.message);
    //     return -1;
    // }
    // logfile.Write("connect database(%s) ok \n",argv[2]);
    
    // // 提交事务
    // conn.commit();

    return 0;
}

// 处理业务的主函数
bool _obtmindtodb(char *pathname, char *connstr, char *charset)
{
    CDir Dir; // 创建目录对象

    // 打开目录，得到数据文件名
    if(Dir.OpenDir(pathname, "*.xml") == false)
    {
        logfile.Write("Dir.OpenDir(%s) failed \n",pathname);
        return false;
    }


    while(true)
    {
        // 读取目录，得到一个数据文件名
        if(Dir.ReadDir() == false) break;

        logfile.Write("filename=%s\n", Dir.m_FullFileName);

        // 打开文件

        // while(true)
        // {
        //     // 处理文件中的每一行
            
        // }
    }

    // 删除文件，提交事务

    return true;
}

void EXIT(int sig)
{
    logfile.Write("程序退出，sig=%d\n\n",sig);

    conn.disconnect();

    exit(0);
}