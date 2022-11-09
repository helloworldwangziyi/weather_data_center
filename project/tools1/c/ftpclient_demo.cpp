#include "_ftp.h"

Cftp ftp;
int main()
{

    if(ftp.login("49.232.5.227:21", "ftpuser", "Wangwang123456") == false)
    {
        printf("ftp.login(49.232.5.227) failed.\n");
        return -1;
    }
    printf("ftp.login(49.232.5.227) ok\n");

    if(ftp.mtime("/home/ftpuser/ftpclient.cpp") == false)
    {
        printf("ftp.mtime(/home/ftpuser/ftpclient.cpp failed \n)");
        return -1;
    }
    printf("read ok\n");

    ftp.logout();
    return 0;
}