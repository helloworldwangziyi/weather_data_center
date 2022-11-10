# 调度程序

## 编写思路

这个程序的功能是每隔一段时间将服务程序重新执行起来，所以被称为调度程序

1. ​    先执行fork函数，创建一个子进程，让子进程，调用execl执行新的程序
2. ​    新的程序将替换子进程，不会影响父进程
3. ​    在父进程中，可以调用wait 函数等待新程序运行的结果，这样就可以实现调度的功能

这里采用exec实现子进程执行程序，这么做节省一个一个写参数的时间

```c++
// 创建子进程失败就退出
if( fork()!= 0 ) exit(0);

// 启用SIGCHLD信号，让父进程可以wait子进程退出的状态
signal(SIGCHLD, SIG_DFL);


char *pargv[argc];
for(int ii = 2; ii < argc; ii++)
{
    pargv[ii-2] = argv[ii];
}
pargv[argc - 2] = NULL;

while(true)
{
    // 子进程执行新的程序
    if( fork() == 0)
    {
        execv(argv[2], pargv);
        // 程序执行失败就退出，否则将会一直fork导致程序崩溃
        exit(0);
    }
    // 父进程在这里等待
    else{ 
        int status;
        wait(&status);
        sleep(atoi(argv[1]));
    }
}
```

这里唯一需要注意的几个概念，**孤儿进程、僵尸进程，还有exec系统调用的使用**

### **孤儿进程**

指的是父进程结束，但是子进程没有结束，导致子进程成为孤儿进程交由系统托管，本身并没有什么危害

### **僵尸进程**

指的是子进程结束，但是父进程没有结束，子进程向父进程发送SIGCHLD信号没有被父进程处理，这样子进程就会成为僵尸进程，由于

操作系统的进程数有上限，所以僵尸进程会对操作系统产生一定的危害

### 避免僵尸进程

僵尸进程会对系统造成压力，那么如何避免出现僵尸进程，我们可以写信号处理函数

```c++
void func(int sig)
{
	int stat;
	wait(&stat);
}

signal(SIGCHLD, func);
```

或者父进程等待子进程退出，并处理，但是父进程要等待就意味着要阻塞，所以可以使用上面的处理方式，这样父进程就不用等待，当信

号到达时表示子进程已经退出这个时候父进程处理信号就ok，还需注意这种方式会产生软中断效果，会打断父进程正在执行的动作，比如

```c++
void func(int sig)
{
	int stat;
	wait(&stat);
}
int main()
{
    signal(SIGCHLD, func);
    
    int pid = fork();
    
    if(pid == 0)
    {
        printf("这是子进程%d， 将执行子进程的任务\n", getpid());
        sleep();
    }
    if(pid > 0)
    {
        printf("这是父进程%d，将执行父进程的任务 \n", getpid());
        // 子进程退出会发送一个软中断信号中断sleep导致退出，若想sleep（10）则需要再写一次
        sleep(10);
        // sleep(10);
    }
}

```

好的，子进程要调度其他程序，就需要写能执行其他程序的系统调用

### exec()家族

这就需要用到execl家族了，其实也就用到了execl 和 execv这两个函数,其他的没用过就不看了

先看看这两个函数的区别

```
The const char *arg and subsequent ellipses in the execl(), execlp(), and execle() functions can be thought
of as arg0, arg1, ..., argn.  Together they describe a list of one  or  more  pointers  to  null-terminated
strings that represent the argument list available to the executed program.  The first argument, by conven‐
tion, should point to the filename associated with the file being executed.  The list of arguments must  be
terminated  by  a NULL pointer, and, since these are variadic functions, this pointer must be cast (char *)
NULL.
```

    The execv(), execvp(), and execvpe() functions provide an array of pointers to null-terminated strings that
    represent  the argument list available to the new program.  The first argument, by convention, should point
    to the filename associated with the file being executed.  The array of pointers must  be  terminated  by  a
    NULL pointer.

上面这段文字的大意就是execl需要将参数一个一个传入，并在最后加上一个(char*)0 作为结束

比如执行ls

```c++
execl("/usr/bin/ls", "/usr/bin/ls", "-lt","/tmp/project.tgz", char*(0));
```

我们的参数有时会很多，一个一个输入太麻烦，就有了execv这种方式，比如参数像这样的

```c++
// /project/tools1/bin/procctl 60 /project/idc1/bin/crtsurfdata5 /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata5.log xml,json,csv
```

```c++
char *pargv[argc];
for(int ii = 2; ii < argc; ii++)
{
    pargv[ii-2] = argv[ii];
}
pargv[argc - 2] = NULL;

while(true)
{
    // 子进程执行新的程序
    if( fork() == 0)
    {
        execv(argv[2], pargv);
        // 程序执行失败就退出，否则将会一直fork导致程序崩溃
        exit(0);
    }
    // 父进程在这里等待
    else{ 
        int status;
        wait(&status);
        sleep(atoi(argv[1]));
    }
}
```

这里建立一个字符串数组用于保存参数，只需注意一点，就是最后要置空！！！ man 手册描述如下

```c++
pargv[argc - 2] = NULL;
```

```
 The array of pointers must  be  terminated  by  a NULL pointer.
```

好的，那么调度程序总结就到这里了