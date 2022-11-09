// 本程序是对mysql数据库进行测试
#include "_public.h"
#include "_mysql.h"

// 帮助文档
void _help();

// 这个用于保存解析出来的数据库连接参数
char buffer[1000];
char char_set[100];


// xml 解析
bool xml_translater(char *xmlbuffer);

// 声明连接对象
connection conn;

int main(int argc, char *argv[])
{
    printf("本程序是对c++操作mysql的详细测试\n");
    if(argc < 2)
    {
        _help();
        return -1;
    }

    xml_translater(argv[1]);
    if(conn.connecttodb(buffer,char_set) != 0)
    {
        printf("数据库连接失败，请检查网络或者参数\n");
    }
    printf("数据库连接成功\n");

    // 接着就是声明我们操作mysql数据库的对象
    sqlstatement stmt(&conn);

    stmt.prepare(argv[2]);
    char res[100];
    stmt.bindout(1, res, 100);
    if (stmt.execute()!=0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
    }
    
   
    

    printf("select table ok\n");

    while(1)
    {
        memset(res,0,sizeof(res));
        if(stmt.next()!=0) break;
        printf("%s\n", res);
    }

    return 0;
}

void _help()
{
    printf("参数输入过少，请输入正确的参数\n");
    printf("example:./mysqlh_select \"<connstr>127.0.0.1,root,Wangwang123456,mysql,3306</connstr><charset>utf8</charset>\"  \"select obtid from T_ZHOBTMIND\" \n\n");
}

bool xml_translater(char *xmlbuffer)
{
    memset(buffer,0,sizeof(buffer));
    memset(char_set,0,sizeof(char_set));
    // 到这里就将参数解析到了buffer中
    GetXMLBuffer(xmlbuffer, "connstr", buffer);
    GetXMLBuffer(xmlbuffer, "charset", char_set);
    printf("%s\n",buffer);
    printf("%s\n",char_set);
    return true;
}