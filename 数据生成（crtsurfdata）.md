# 生成气象中心数据：

## 数据说明：

数据分为两类：

​		第一类是全国气象站点参数数据：全国一共839个气象站，数据保存在/project/idc/ini/stcode.ini

​		第二类是这些站点的分钟观测数据，这些站点每分钟进行一次观测，每次观测产生一共839条数据，由于拿不到正式的气象观测数据

我们就采用程序进行模拟

业务要求：

1. 根据全国气象站点参数，模拟生成观测数据
2. 程序每分钟运行一次，每次生成839条数据，存放在一个文件中

## 程序的主要思路是：

1. 给出参数的说明文档
2. 打开日志文件
3. 把站点参数文件加载到容器
4. 获取当前时间作为观测时间
5. 模拟生成全国气象站点分钟观测数据，存放在容器中
6. 把容器中的全国气象站点分钟观测数据写入文件

我们重点要实现的是3，4，5，6

同时给出全国气象站点参数结构体和全国气象站点分钟观测数据结构

```c++
// 全国气象站点参数结构体
struct st_stcode
{
    /* data */
    char provname[31]; // 省
    char obtid[11];    // 站号
    char obtname[31];  // 站名                       
    double lat;        // 纬度
    double lon;        // 经度 
    double height;     // 海拔高度
};

// 全国气象站点分钟观测数据结构
struct st_surfdata
{
  char obtid[11];      // 站点代码。
  char ddatetime[21];  // 数据时间：格式yyyymmddhh24miss
  int  t;              // 气温：单位，0.1摄氏度。
  int  p;              // 气压：0.1百帕。
  int  u;              // 相对湿度，0-100之间的值。
  int  wd;             // 风向，0-360之间的值。
  int  wf;             // 风速：单位0.1m/s
  int  r;              // 降雨量：0.1mm。
  int  vis;            // 能见度：0.1米。
};
```

这个文件编写的整体思路就是，

首先将站点参数文件加载到容器中，对应实现下面这个函数和容器

```c++
// 把站点参数文件加载到vstcode容器中
bool LoadSTCode(const char *inifile);

// 存放全国气象站点参数的容器
vector<struct st_stcode> vstcode;
```

然后再遍历容器生成分钟观测数据加载到对应容器中，

```c++
// 模拟生成全国气象站分钟观测数据，存放在vsurfdata中
void CrtSurfData();

// 存放全国气象观测分钟数据的容器
vector<struct st_surfdata>vsurfdata;

// 模拟生成站点观测数据是 依靠随机数自动生成
// 用随机数填充分钟观测数据的结构体
strncpy(stsurfdata.obtid, vstcode[ii].obtid, 10); // 站点代码
strncpy(stsurfdata.ddatetime, strddatetime, 14);  // 数据时间： 格式 yyyymmddhh24miss
stsurfdata.t = rand() % 351;                      // 气温 单位0.1 摄氏度
stsurfdata.p = rand() % 265 + 10000;              // 气压 0.1 百帕
stsurfdata.u = rand() % 100 + 1;                  // 相对湿度 0-100 之间的值
stsurfdata.wd = rand() % 360;                      // 风向 0-360之间的值
stsurfdata.wf = rand() % 150;                      // 风速 单位 0.1m/s
stsurfdata.r = rand() % 16;                       // 降雨量 0.1mm
stsurfdata.vis = rand() % 5001 + 100000;            // 能见度 0.1米
```

然后再将容器中的数据写入对应文件中即可！

```c++
// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件
bool CrtSurfFile(const char *outpath, const char * datafmt);
```

这一部分模块还加上生成历史数据文件的功能

```c++
// 获取当前时间，作为观测事件
memset(strddatetime, 0 ,sizeof(strddatetime));
// 等于5的时候就是获取当前时间为观测时间
if (argc == 5)
    LocalTime(strddatetime, "yyyymmddhh24miss");
else
   	// 指定第6个参数为历史观测时间 
    STRCPY(strddatetime, sizeof(strddatetime), argv[5]);
```

要注意写入文件时一定要小心，将生成的数据从对应结构体插入到csv，xml，json中的代码块，这一部分并不难但是容易写错

```c++
    // 写入第一行的标题
    if (strcmp(datafmt, "csv") == 0) File.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");
    if (strcmp(datafmt,"xml")==0) File.Fprintf("<data>\n");
    if (strcmp(datafmt,"json")==0) File.Fprintf("{\"data\":[\n");
    // 遍历存放观测数据的vsurfdata容器
    for(int ii = 0; ii < vsurfdata.size(); ii++)
    {
        // 写入一条数据
        if(strcmp(datafmt, "csv") == 0)
        {
            File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, \
                vsurfdata[ii].t / 10.0, vsurfdata[ii].p / 10.0, vsurfdata[ii].u,vsurfdata[ii].wd, vsurfdata[ii].wf / 10.0, \
                vsurfdata[ii].r / 10.0, vsurfdata[ii].vis / 10.0);
        }
        if (strcmp(datafmt,"xml")==0)
            File.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p>"\
                   "<u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",\
                    vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
                    vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
        if (strcmp(datafmt,"json")==0)
        {
            File.Fprintf("{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\","\
                   "\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":\"%.1f\"}",\
                vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
                vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
            if (ii<vsurfdata.size()-1) File.Fprintf(",\n");
            else   File.Fprintf("\n");
        }
    }
    // sleep(50);
    if (strcmp(datafmt,"xml")==0) File.Fprintf("</data>\n");
    if (strcmp(datafmt,"json")==0) File.Fprintf("]}\n");
```

最后一点，**文件写入并不是直接写入文件，而是先创建临时文件**，将数据先写入临时文件，最后再将临时文件转成正式文件（这么做是为

了保证数据写入时被读取的问题）！！！

文件生成代码属于比较重要的功能，所以应当由守护进程进行监视。