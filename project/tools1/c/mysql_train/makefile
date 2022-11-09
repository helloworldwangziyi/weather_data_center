# 开发框架头文件路径
PUBINCL = -I/project/public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试
PUBCPP = /project/public/_public.cpp

# mysql头文件存放路径
MYSQLINCL = -I/usr/include/mysql  -I/project/public/db/mysql

# mysql库文件保存路径
MYSQLLIB = -L/usr/lib64/mysql

# mysql链接库
MYSQLLIBS = -lmysqlclient

# 开发框架mysql的cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
MYSQLCPP = /project/public/db/mysql/_mysql.cpp

# 编译参数
CFLAGS = -g

all:mysqlh_using


mysqlh_using:mysqlh_using.cpp
	g++ $(CFLAGS) -o mysqlh_using mysqlh_using.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc



clean:
	rm mysqlh_using