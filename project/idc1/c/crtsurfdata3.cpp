// 全国气象站参数，全国一共有839个气象站，数据保存在文本文件中
// 数据保存在  /project/idc/ini/stcode.ini

// 这些站点每分钟进行一次观测，每次观测会产生一共839条数据，由于拿不到正式的气象观测数据，所以我们采用程序进行模拟
// 业务要求：
    // 根据全国气象站点参数，模拟生成观测数据
    // 程序每分钟运行一次，每次生成839条数据，存放在一个文件中

/*
程序名： crtsurfdata3.cpp 本程序用于生成全国气象站点观测的分钟数据
作者： 王梓伊
*/

#include "_public.h"

// 创建日志类 日志文件大小默认为100M 缺省情况下当文件超过100M时会进行切换
CLogFile logfile;

// 安徽,58015,砀山,34.27,116.2,44.2
// 使用结构体来保存上面的全国气象站点数据
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
// 存放全国气象站点参数的容器
vector<struct st_stcode> vstcode;

// 把站点参数文件加载到vstcode容器中
bool LoadSTCode(const char *inifile);

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

// 存放全国气象观测分钟数据的容器
vector<struct st_surfdata>vsurfdata;

// 模拟生成全国气象站分钟观测数据，存放在vsurfdata中
void CrtSurfData();


int main(int argc, char *argv[])
{
    // 全国气象站点参数文件 输出的分钟数据存放路径， 日志文件
    // inifile outpath  logfile
    if(argc != 4)
    {
        printf("Using :./crtsurfdata3 inifile outpath logfile\n");
        printf("Example:/project/idc1/bin/crtsurfdata3 /project/idc1/ini/stcode.ini /tmp/surdata /log/idc/crtsurfdata3.log \n\n");
        
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
    logfile.Write("crtsurfdata3 开始运行 \n");
   
    // 把站点参数文件中加载到vstcode容器中,这里加载失败程序退出
    if(LoadSTCode(argv[1]) == false) return -1;
    
    // 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中
    CrtSurfData();

    logfile.WriteEx("crtsurfdata3 运行结束 \n ");
    return 0;
}

// 把站点参数文件加载到vstcode中
bool LoadSTCode(const char *inifile)
{
    // 操作文件的类
    CFile File;
    // 打开站点参数文件
    if(File.Open(inifile, "r") == false)
    {   
        // 如果打开失败就写进日志并返回false
        logfile.Write("File.Open(%s) failed \n",inifile); return false;
    }
    // 每次读取的结果存放在这里
    char strBuffer[301];

    // 拆分字符串的类
    CCmdStr CmdStr;

    struct st_stcode stcode;

    while(true)
    {
        // 从站点参数文件中读取一行，如果已经读取完，跳出循环(这里失败表示文件已经结束)
        if(File.Fgets(strBuffer, 300 , true) == false) break;
        
        logfile.Write("=%s=\n", strBuffer);
        // 把读取道德一行进行拆分
        CmdStr.SplitToCmd(strBuffer, ",", true);

        if(CmdStr.CmdCount()!=6) continue; // 扔掉无用的行

        // 把站点参数的每个数据项保存到站点参数结构体中
        CmdStr.GetValue(0, stcode.provname, 30); // 省
        CmdStr.GetValue(1, stcode.obtid, 30);    // 站号
        CmdStr.GetValue(2, stcode.obtname, 30);  // 站名
        CmdStr.GetValue(3, &stcode.lat);         // 纬度  
        CmdStr.GetValue(4, &stcode.lon);         // 经度
        CmdStr.GetValue(5, &stcode.height);      // 海拔高度
        // 把站点参数结构体放入站点参数容器
        vstcode.push_back(stcode);
    }
    // for (int ii=0;ii<vstcode.size();ii++)
    //     logfile.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",\
    //                vstcode[ii].provname,vstcode[ii].obtid,vstcode[ii].obtname,vstcode[ii].lat,\
    //                vstcode[ii].lon,vstcode[ii].height);

    return true;
}

// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中
void CrtSurfData()
{
    // 播下随机数的种子
    srand(time(0));
    // 获取当前时间，作为观测事件
    char strddatetime[21];
    memset(strddatetime, 0 ,sizeof(strddatetime));
    LocalTime(strddatetime, "yyyymmddhh24miss");
    // 遍历气象站点参数的容器
    struct st_surfdata stsurfdata;

    for(int ii = 0; ii < vstcode.size(); ii++)
    {
        memset(&stsurfdata, 0, sizeof(stsurfdata));

        // 用随机数填充分钟观测数据的结构体
        strncpy(stsurfdata.obtid, vstcode[ii].obtid, 10); // 站点代码
        strncpy(stsurfdata.ddatetime, strddatetime, 14);  // 数据时间： 格式 yyyymmddhh24miss
        // 这里就是自己写的时候很不认真
        stsurfdata.t = rand() % 351;                      // 气温 单位0.1 摄氏度
        stsurfdata.p = rand() % 265 + 10000;              // 气压 0.1 百帕
        stsurfdata.p = rand() % 100 + 1;                  // 相对湿度 0-100 之间的值
        stsurfdata.p = rand() % 360;                      // 风向 0-360之间的值
        stsurfdata.p = rand() % 150;                      // 风速 单位 0.1m/s
        stsurfdata.p = rand() % 16;                       // 降雨量 0.1mm
        stsurfdata.p = rand() % 5001 + 100000;            // 能见度 0.1米

        // 把观测数据的结构体放入vsurfdata容器
        vsurfdata.push_back(stsurfdata);

    }


}