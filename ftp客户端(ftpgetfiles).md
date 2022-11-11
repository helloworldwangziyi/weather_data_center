# FTP 客户端程序

## 编写思路

本程序实现ftp客户端增量下载功能，此程序的难点就是在增量下载上，增量下载的实现可以借用四个vector容器，如下图所示，list2存放

服务器上待下载的文件，list1存放已经下载的文件，之后将list1 和list2 进行对比，list1 和list2中都有的文件写入list3中，list4中存放本次

需要下载的文件名。

![image-20221111100548701](C:\Users\王梓伊\AppData\Roaming\Typora\typora-user-images\image-20221111100548701.png)

我们实现这个功能是由易到难，所以这里从全量下载到增量下载逐步实现，上面的是增量下载的思路简介

 程序所用到的结构体用于存放参数

```c++
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
    bool checkmtime;         // 是否需要检查服务端的时间 true-需要 false -不需要
    int timeout;             // 进程心跳的超时时间
    char pname[51];          // 进程名，建议用ftpgetfiles_后缀的方式
} starg;


// 文件信息的结构体
struct st_fileinfo
{
    char filename[301]; // 文件名
    char mtime[21];     // 文件时间
};

vector<struct st_fileinfo> vlistfile1; // 已下载成功文件名的容器
vector<struct st_fileinfo> vlistfile2; // 存放下载前列出服务器文件名的窗口
vector<struct st_fileinfo> vlistfile3; // 本次不需要下载的文件的容器
vector<struct st_fileinfo> vlistfile4; // 本次需要下载的文件的容器
```

编写程序的整体思路

1. 判断参数个数，加载说明文档
2. 关闭信号以及IO
3. 打开日志文件
4. 解析xml（传入参数）
5. 登录ftp服务器
   1. 进入ftp服务器存放文件的目录
   2. 调用ftp.nlist列出服务器目录中的文件，结果存放到本地文件中
   3. 把获取到的list文件加载到容器vfilelist中
   4. 判断starg.ptype的值
      1. starg.ptype等于1 表示增量下载
         1. 加载已下载成功的文件清单到容器vlistfile1中
         2. 比较vlistfile2 和vlistfile1得到vlistfile3 和 vlistfile4
         3. 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的okfilename文件
         4. 把vlistfile4中的内容复制到vlistfile2中

之后就是下载，删除和备份操作比较简单不再详述，代码中注释表明的很清楚

这个程序的最困难的实现之处就是实现增量下载，这个思路整理清楚ftp下载就ok，最后注意一点这里实现增量不仅仅只靠文件名，**还靠文**

**件时间，文件时间来决定一个文件是否做过修改**，如果做对比时文件修改时间发现不同，那么这个文件就应当加入下载队列！！





​			





