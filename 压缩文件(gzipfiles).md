# 压缩文件

## 编写思路

与删除文件的编写思路相同，同样是压缩超过时间的文件，压缩操作依据时间进行

程序结构十分简单，唯一需要注意的是压缩操作的命令，以及如何执行

```c++
SNPRINTF(strCmd,sizeof(strCmd), 1000,"/usr/bin/gzip -f %s 1>/dev/null 2>/dev/null",Dir.m_FullFileName);
if(system(strCmd) == 0)
{
    printf("gzip %s is ok .\n", Dir.m_FullFileName);
}
else{
    printf("gzip %s is failed .\n", Dir.m_FullFileName);
}
```

来看上面的代码第一句拼接待执行的命令，执行压缩命令，并将标准错误，标准输出输入到黑洞，这里我们采用了system来执行，这与

exec家族有什么区别？

**system 是在单独的进程中执行命令，完了还会回到你的程序中。而*exec*函数是直接在你的进程中执行新的程序，新的程序会把你的程序**

**覆盖，除非调用出错，否则你再也回不到*exec*后面的代码，就是说你的程序就变成了*exec*调用的那个程序了。**

