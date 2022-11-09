// 全国气象站参数，全国一共有839个气象站，数据保存在文本文件中
// 数据保存在  /project/idc/ini/stcode.ini

// 这些站点每分钟进行一次观测，每次观测会产生一共839条数据，由于拿不到正式的气象观测数据，所以我们采用程序进行模拟
// 业务要求：
    // 根据全国气象站点参数，模拟生成观测数据
    // 程序每分钟运行一次，每次生成839条数据，存放在一个文件中

/*
程序名： crtsurfdata1.cpp 本程序用于生成全国气象站点观测的分钟数据
作者： 王梓伊
*/

#include "_public.h"

// 创建日志类 日志文件大小默认为100M 缺省情况下当文件超过100M时会进行切换
CLogFile logfile;

int main(int argc, char *argv[])
{
    // 全国气象站点参数文件 输出的分钟数据存放路径， 日志文件
    // inifile outpath  logfile

    if(argc != 4)
    {
        printf("Using :./crtsurfdata1 inifile outpath logfile\n");
        printf("Example:/project/idc1/bin/crtsurfdata1 /project/idc1/ini/stcode.ini /tmp/surdata /log/idc/crtsurfdata1.log \n\n");
        
        printf("inifile 全国气象站点参数文件名 \n");
        printf("outpath 全国气象站点数据文件存放的目录 \n");
        printf("logfile 本程序运行的日志文件名 \n\n");

        return -1;

    }

    if(logfile.Open(argv[3]) == false)
    {
        // 打开日志文件失败就退出程序就不会继续运行
        printf("logfile.Open(%s) failed./n", argv[3]); return -1;   
    }
    logfile.Write("crtsurfdata1 开始运行 \n");

    // 这里插入运行代码


    return 0;
}