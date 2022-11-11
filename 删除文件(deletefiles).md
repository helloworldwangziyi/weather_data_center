# 删除文件（deletefiles）

## 编写思路：

本程序实现删除时间过长的文件

既然是依据时间进行删除，所以参数值中的时间值就是重点，我们要获取文件超时时间点，并遍历目录与每个文件做对比，删除超时文件

```c++
/project/tools1/bin/deletefiles /log/idc \"*.log.20*\" 0.02
```

首先看上面的文件运行参数最后一个就是文件时间0.02 代表0.02天，我们删除时间标识超过0.02天的文件

```c++
// 获取文件超时的时间点。
char strTimeOut[21];
LocalTime(strTimeOut,"yyyy-mm-dd hh24:mi:ss",0-(int)(atof(argv[3])*24*60*60));
```

这个操作是靠上面这一步实现的，这里我们用strTimeOut字符串变量来获取时间偏移量

```c++
if (strcmp(Dir.m_ModifyTime,strTimeOut)<0) 
```

之后就依赖上面这句对比来判断文件是否超时，小于0代表超时，如果实在不理解就将这些关键变量打印出来，写入日志

这个程序的编写应该没有难度