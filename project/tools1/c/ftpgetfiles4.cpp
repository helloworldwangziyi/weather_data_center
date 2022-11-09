#include "_public.h"
#include "_ftp.h"

// 创建日志对象
CLogFile logfile;
// 创建ftp对象
Cftp ftp;

// 程序退出和信号2、15的处理函数
void EXIT(int sig);

// 程序的说明文档
void _help();

//  保存参数的结构体
struct st_arg
{
    char host[31];           // 远程服务器的IP和端口。
    int  mode;               // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
    char username[31];       // 远程服务器ftp的用户名。
    char password[31];       // 远程服务器ftp的密码。
    char remotepath[301];    // 远程服务器存放文件的目录。
    char localpath[301];     // 本地文件存放的目录。
    char matchname[101];     // 待下载文件匹配的规则。
    char listfilename[301];  // 下载前列出服务器文件名的文件。
    int ptype;               // 文件下载成功后，远程服务器的处理方式：1-什么也不做 2-删除 3-备份 如果为3还要指定备份的目录
    char remotepathbak[301]; // 文件下载成功后，远程服务器的备份目录，参数只有当ptype =3时生效
    char okfilename[301];    // 已下载成功文件名清单
} starg;


// 文件信息的结构体
struct st_fileinfo
{
    char filename[301]; // 文件名
    char mtime[21];     // 文件时间
};


// 把ftp.nlist() 获取到的list文件加载到vfilelist中
bool LoadListFile();

// 加载okfilename文件中的容器到容器vlistfile1中
bool LoadOKFile();
// 比较vlistfile2和vlistfile1 得到vlistfile3和vlistfile4
bool CompVector();
// 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的旧的okfilename文件
bool WriteToOKFile();
// 如果ptype == 1 把下载成功的文件记录追加到okfilename文件中
bool AppendToOKFile(struct st_fileinfo *stfilename);

vector<struct st_fileinfo> vlistfile1; // 已下载成功文件名的容器
vector<struct st_fileinfo> vlistfile2; // 存放下载前列出服务器文件名的窗口
vector<struct st_fileinfo> vlistfile3; // 本次不需要下载的文件的容器
vector<struct st_fileinfo> vlistfile4; // 本次需要下载的文件的容器

// xml 解析函数 ，将xml解析到starg中
bool _xmltoarg(char *strxmlbuffer);

// 下载文件功能的主函数
bool _ftpgetfiles();

int main(int argc, char *argv[])
{

    // 说明文档
    if(argc < 3 )
    {
        _help();
        return -1;
    }

    // 处理程序的退出信号
    // 关闭全部信号的输入输出
    // CloseIOAndSignal();
    signal(SIGINT,EXIT);
    signal(SIGTERM,EXIT);

    // 打开日志文件
    if(logfile.Open(argv[1], "a+") == false)
    {
        printf("打开日志文件（%s）\n", argv[1]);
        return -1;
    }

    // 解析xml 得到程序的运行参数  
    if (_xmltoarg(argv[2])==false) return -1;
    
    // 登录ftp服务器
    if(ftp.login(starg.host, starg.username, starg.password,starg.mode) == false)
    {
        logfile.Write("ftp.logini(%s, %s, %s) failed \n", starg.host, starg.username, starg.password);
        return -1;
    }
    // 提示登录成功
    logfile.Write("ftp.login ok \n");
    
    // 下载文件主函数
    _ftpgetfiles();

    // 等出
    ftp.logout();

    return 0;
}

// 把ftp.nlist()方法获取到的list文件加载到容器vfilelist中
bool LoadListFile()
{
    // 使用前先将容器 清空
    vlistfile2.clear();

    // 创建文件对象
    CFile File;

    if(File.Open(starg.listfilename, "r")== false)
    {
        logfile.Write("File Open(%s) 失败 \n",starg.listfilename);
        return false;
    }
    
    struct st_fileinfo stfileinfo;

    while(true)
    {
        memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
        // 读取失败表示文件读取完毕,最后一个参数表示去除换行符\r\n
        if(File.Fgets(stfileinfo.filename, 300 , true) == false) break;

        // 如果名称不匹配就跳过
        if(MatchStr(stfileinfo.filename,starg.matchname) == false) continue;

        vlistfile2.push_back(stfileinfo);  
    }

    // for(int ii = 0; ii < vlistfile2.size(); ii++)
    // {
    //     logfile.Write("filename=%s=\n",vlistfile2[ii].filename);
    // }
    return true;
}



// 下载文件功能的主函数
bool _ftpgetfiles()
{
    // 进入ftp服务器存放文件的目录
    if(ftp.chdir(starg.remotepath) == false)
    {
        logfile.Write("ftp.chdir(%s) failed \n", starg.remotepath);
        return false;
    }

    // 调用ftp.nlist() 方法列出服务器目录中的文件，结果存放到本地文件中
    if(ftp.nlist(".", starg.listfilename) == false)
    {
        logfile.Write("ftp.nlist(%s) failed \n", starg.listfilename);
        return false;
    }
    // 把获取到的list文件加载到容器vfilelist中
    if(LoadListFile() == false)
    {
        logfile.Write("LoadListFile() failed \n");
        return false;
    }

    // 增量下载
    if(starg.ptype == 1)
    {
        // 加载okfilename文件中的内容到容器vlistfile1中
        LoadOKFile();

        // 比较vlistfile2 和vlistfile1 得到vlistfile3和vlistfile4
        CompVector();

        // 把容器vlistfile3中的内容写入okfilename文件，覆盖之前旧的okfilename文件
        WriteToOKFile();

        // 把vlistfile4中的内容复制到vlistfile2中
        vlistfile2.clear();
        vlistfile2.swap(vlistfile4);
    }
        
    // 遍历容器vfilelist
    char strremotefilename[301], strlocalfilename[301];
    for(int ii = 0; ii < vlistfile2.size(); ii++)
    {
        SNPRINTF(strremotefilename,sizeof(strremotefilename),300,"%s/%s",starg.remotepath,vlistfile2[ii].filename);
        SNPRINTF(strlocalfilename,sizeof(strlocalfilename),300,"%s/%s",starg.localpath,vlistfile2[ii].filename);
        
        // 调用ftp.get()方法从服务器下载文件
        logfile.Write("get %s ...", strremotefilename);
        if(ftp.get(strremotefilename,strlocalfilename) == false)
        {
            logfile.Write("failed \n");
            break;
        } 
        logfile.WriteEx("ok \n");

        // 如果ptype == 1 把下载成功的文件追加到okfilename文件中
        if(starg.ptype == 1) AppendToOKFile(&vlistfile2[ii]);

        // 删除ftp服务器上的文件
        if (starg.ptype == 2)
        {
            ftp.ftpdelete(strremotefilename);
        }
        // 备份ftp服务器上的文件
        if(starg.ptype == 3)
        {
            char strremotefilenamebak[301];
            SNPRINTF(strremotefilenamebak,sizeof(strremotefilenamebak),300,"%s/%s",starg.remotepathbak,vlistfile2[ii].filename);
            if (ftp.ftprename(strremotefilename,strremotefilenamebak)==false)
            {
                logfile.Write("ftp.ftprename(%s,%s) failed.\n",strremotefilename,strremotefilenamebak); return false;
            }

        }

    }
    return true;
}

// 信号处理函数
void EXIT(int sig)
{
    printf("程序退出, sig=%d\n\n", sig);   
    exit(0);
}

// 程序的帮助文档
void _help()
{
    printf("\n");
    printf("Using:/project/tools1/bin/ftpgetfiles logfilename xmlbuffer\n\n");

    printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log \"<host>127.0.0.1:21</host><mode>1</mode><username>ftpuser</username><password>Wangwang123456</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename>\"\n\n\n");

    printf("本程序是通用的功能模块，用于把远程ftp服务器的文件下载到本地目录。\n");
    printf("logfilename是本程序运行的日志文件。\n");
    printf("xmlbuffer为文件下载的参数，如下：\n");
    printf("<host>127.0.0.1:21</host> 远程服务器的IP和端口。\n");
    printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
    printf("<username>wucz</username> 远程服务器ftp的用户名。\n");
    printf("<password>wuczpwd</password> 远程服务器ftp的密码。\n");
    printf("<remotepath>/tmp/idc/surfdata</remotepath> 远程服务器存放文件的目录。\n");
    printf("<localpath>/idcdata/surfdata</localpath> 本地文件存放的目录。\n");
    printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> 待下载文件匹配的规则。"\
            "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
    printf("<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename> 下载前列出服务器文件名的文件。\n");
    printf("<ptype>1</ptype> 文件下载成功后，远程服务器的处理方式: 1-什么也不做; 2-删除; 3-备份,如果为三，还要指定备份的目录\n");
    printf("<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> 文件下载成功后，服务器文件备份目录，此参数只有当ptype=3时才有效\n");
    printf("<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename> 已下载成功清单此参数只有当ptype=1时有效 \n\n\n");

}


// 把xml解析到参数starg结构中。
bool _xmltoarg(char *strxmlbuffer)
{
    memset(&starg,0,sizeof(struct st_arg));

    GetXMLBuffer(strxmlbuffer,"host",starg.host,30);   // 远程服务器的IP和端口。
    if (strlen(starg.host)==0)
    { logfile.Write("host is null.\n");  return false; }

    GetXMLBuffer(strxmlbuffer,"mode",&starg.mode);   // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
    if (starg.mode!=2)  starg.mode=1;

    GetXMLBuffer(strxmlbuffer,"username",starg.username,30);   // 远程服务器ftp的用户名。
    if (strlen(starg.username)==0)
    { logfile.Write("username is null.\n");  return false; }

    GetXMLBuffer(strxmlbuffer,"password",starg.password,30);   // 远程服务器ftp的密码。
    if (strlen(starg.password)==0)
    { logfile.Write("password is null.\n");  return false; }

    GetXMLBuffer(strxmlbuffer,"remotepath",starg.remotepath,300);   // 远程服务器存放文件的目录。
    if (strlen(starg.remotepath)==0)
    { logfile.Write("remotepath is null.\n");  return false; }

    GetXMLBuffer(strxmlbuffer,"localpath",starg.localpath,300);   // 本地文件存放的目录。
    if (strlen(starg.localpath)==0)
    { logfile.Write("localpath is null.\n");  return false; }

    GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);   // 待下载文件匹配的规则。
    if (strlen(starg.matchname)==0)
    { logfile.Write("matchname is null.\n");  return false; }

    GetXMLBuffer(strxmlbuffer,"listfilename",starg.listfilename,300);   // 下载前列出服务器文件名的文件
    if (strlen(starg.listfilename)==0)
    { logfile.Write("listfilename is null.\n");  return false; }

    // 下载后服务器文件的处理方式  1-什么也不做; 2-删除; 3-备份,如果为三，还要指定备份的目录
    GetXMLBuffer(strxmlbuffer,"ptype", &starg.ptype);   
    if( (starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype!= 3))
    { logfile.Write("ptype is error.\n");  return false; }

    // 下载后文件服务器备份目录
    GetXMLBuffer(strxmlbuffer,"remotepathbak",starg.remotepathbak,300);   
    if ((starg.ptype == 3) && (strlen(starg.remotepathbak)==0))
    { logfile.Write("remotepathbak is null.\n");  return false; }


    GetXMLBuffer(strxmlbuffer,"okfilename",starg.okfilename,300);   // 已下载成功文件名 清单
    if ((starg.ptype== 1) && (strlen(starg.okfilename)==0))
    { logfile.Write("okfilename is null.\n");  return false; }

    return true;
}


// 加载okfilename文件中的容器到容器vlistfile1中
bool LoadOKFile()
{
    vlistfile1.clear();
    
    CFile File;

    // 程序第一次运行okfilename根本不存在，所以一定会打开失败,这个并不算是错误
    if(File.Open(starg.okfilename, "r") == false)
    {
        return true;
    }

    struct st_fileinfo stfileinfo;

    while (true)
    {
        memset(&stfileinfo,0,sizeof(struct st_fileinfo));

        if (File.Fgets(stfileinfo.filename,300,true)==false) break;

        vlistfile1.push_back(stfileinfo);
    }

    return true;
}

// 比较vlistfile2和vlistfile1 得到vlistfile3和vlistfile4
bool CompVector()
{
    vlistfile3.clear();
    vlistfile4.clear();
    int ii;
    int jj;

    for(ii =0; ii<vlistfile2.size(); ii++)
    {
        for(jj = 0; jj < vlistfile1.size(); jj++)
        {
            // 如果找到，把记录放入vlistfile3
            if(strcmp(vlistfile2[ii].filename, vlistfile1[jj].filename) == 0)
            {
                vlistfile3.push_back(vlistfile2[ii]);
                break;
            }
        }
        // 如果没有找到，把记录放入vlistfile4
        if (jj == vlistfile1.size()) vlistfile4.push_back(vlistfile2[ii]);
    }

    return true;
}

// 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的旧的okfilename文件
bool WriteToOKFile()
{
    CFile File;

    if(File.Open(starg.okfilename,"w") == false)
    {
        logfile.Write("File.Open(%s) failed \n", starg.okfilename);
        return false;
    }
    for(int ii = 0; ii < vlistfile3.size(); ii++)
    {
        File.Fprintf("%s\n", vlistfile3[ii].filename);
    }

    return true;
}

// 如果ptype == 1，把下载成功的文件记录追加到okfilename文件中
bool AppendToOKFile(struct st_fileinfo *stfilename)
{
    CFile File;

    if(File.Open(starg.okfilename,"a") == false)
    {
        logfile.Write("File.Open(%s) failed.\n", starg.okfilename);
        return false;
    }

    File.Fprintf("%s\n",stfilename->filename);
    return true;
}






