/*
 obcodetodb.cpp 本程序用于把全国站点分钟观测数据保存到数据库T_ZHUOBTCODE表中，支持xml和csv两种文件格式
 作者：王梓伊
*/
#include "_public.h"
#include "_mysql.h"


struct st_zhobtmind
{
    char obtid[11];     // 站点代码
    char ddatetime[21]; // 数据时间，精确到分钟
    char t[11];         // 温度，单位：0.1摄氏度
    char p[11];         // 气压，单位：0.1百帕
    char u[11];         // 相对湿度，0-100之间的值
    char wd[11];        // 风向，0-360之间的值
    char wf[11];        // 风速：单位0.1m/s
    char r[11];         // 降雨量：0.1mm
    char vis[11];       // 能见度 ：0.1m
}stzhobtmind;

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

    return 0;
}

// 处理业务的主函数
bool _obtmindtodb(char *pathname, char *connstr, char *charset)
{
    // 准备插入表的sql语句
    sqlstatement stmt;
    
    
    CDir Dir; // 创建目录对象

    // 打开目录，得到数据文件名
    if(Dir.OpenDir(pathname, "*.xml") == false)
    {
        logfile.Write("Dir.OpenDir(%s) failed \n",pathname);
        return false;
    }

    CFile File;

    int totalcount = 0;  // 文件的总记录数
    int insertcount = 0; // 成功插入记录数
    CTimer Timer;        // 计时器，记录每个数据文件的处理耗时

    while(true)
    {

        // 读取目录，得到一个数据文件名
        if(Dir.ReadDir() == false) break;

        // 链接数据库
        if(conn.m_state ==0)
        {
            if(conn.connecttodb(connstr, charset) != 0)
            {
                logfile.Write("connect database(%s) failed.\n%s\n",connstr, conn.m_cda.message);
                return -1;
            }
            logfile.Write("connect database(%s) ok \n",connstr);
        }

        if(stmt.m_state==0)
        {
            stmt.connect(&conn);
            stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis) values(:1, str_to_date(:2, '%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9)");
            stmt.bindin(1, stzhobtmind.obtid,10);
            stmt.bindin(2, stzhobtmind.ddatetime,14);
            stmt.bindin(3, stzhobtmind.t,10);
            stmt.bindin(4, stzhobtmind.p,10);
            stmt.bindin(5, stzhobtmind.u,10);
            stmt.bindin(6, stzhobtmind.wd,10);
            stmt.bindin(7, stzhobtmind.wf,10);
            stmt.bindin(8, stzhobtmind.r,10);
            stmt.bindin(9, stzhobtmind.vis,10);
        }
        // logfile.Write("filename=%s\n", Dir.m_FullFileName);
        totalcount= insertcount=0;
        // 打开文件
        if(File.Open(Dir.m_FullFileName, "r") == false)
        {
            logfile.Write("File.Open(%s) failed \n",Dir.m_FullFileName);
            return false;
        }

        char strBuffer[1001]; // 存取从文件中读取的一行

        while(true)
        {
            if(File.FFGETS(strBuffer, 1000, "<endl/>") == false) break;
            // logfile.Write("strBuffer=%s", strBuffer);

            // 处理文件中的每一行
            totalcount++;
            memset(&stzhobtmind, 0, sizeof(struct st_zhobtmind));
            GetXMLBuffer(strBuffer,"obtid", stzhobtmind.obtid, 10);
            GetXMLBuffer(strBuffer,"ddatetime", stzhobtmind.ddatetime, 14);
            char tmp[11];
            GetXMLBuffer(strBuffer,"t", stzhobtmind.t, 10);
            if(strlen(tmp) > 0) snprintf(stzhobtmind.t,10,"%d",(int)(atof(tmp)*10));
            GetXMLBuffer(strBuffer,"p", stzhobtmind.p, 10);
            GetXMLBuffer(strBuffer,"u", stzhobtmind.u, 10);
            GetXMLBuffer(strBuffer,"wd", stzhobtmind.wd, 10);
            GetXMLBuffer(strBuffer,"wf", stzhobtmind.wf, 10);
            if(strlen(tmp) > 0) snprintf(stzhobtmind.wf,10,"%d",(int)(atof(tmp)*10));
            GetXMLBuffer(strBuffer,"r", stzhobtmind.r, 10);
            if(strlen(tmp) > 0) snprintf(stzhobtmind.r,10,"%d",(int)(atof(tmp)*10));
            GetXMLBuffer(strBuffer,"vis", stzhobtmind.vis, 10);
            if(strlen(tmp) > 0) snprintf(stzhobtmind.vis,10,"%d",(int)(atof(tmp)*10));

            // logfile.Write("obtid=%s,ddatetime=%s,t=%s,p=%s,u=%s,wd=%s,wf=%s,r=%s,vis=%s\n",stzhobtmind.obtid,stzhobtmind.ddatetime,stzhobtmind.t,stzhobtmind.p,stzhobtmind.u,stzhobtmind.wd,stzhobtmind.wf,stzhobtmind.r,stzhobtmind.vis);
        
            // 将结构体中的数据插入表中
            if(stmt.execute()!= 0)
            {

                // 失败的情况由哪些？ 是否需要全部写日志
                // 失败的主要原因有两点：1是记录重复 2是数据内容非法
                // 如果失败了怎么办？程序是否需要继续？是否rollback 是否返回false
                // 如果失败的原因是因为数据内容非法，记录日之后继续 如果是记录重复，不必记录日志，且继续

                if(stmt.m_cda.rc!= 1062)
                {
                    logfile.Write("Buffer= %s\n", strBuffer);
                    logfile.Write("stmt.execute() failed \n%s\n%s\n",stmt.m_sql,stmt.m_cda.message);
                }
                
            }
            else
            {
                insertcount++;
            }
        
        }
        
        // 删除文件、提交事务。
        // File.CloseAndRemove();
        conn.commit();
        logfile.Write("已处理文件%s (totalcount=%d, insertcount=%d), 耗时%.2f秒 \n", Dir.m_FullFileName,totalcount, insertcount, Timer.Elapsed());
    }

    return true;
}

void EXIT(int sig)
{
    logfile.Write("程序退出，sig=%d\n\n",sig);

    conn.disconnect();

    exit(0);
}