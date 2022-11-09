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

all: procctl checkproc gzipfiles deletefiles ftpclient_demo ftpgetfiles ftpputfiles tcpputfiles tcpgetfiles fileserver execsql\
	dminingmysql xmltodb syncupdate

procctl:procctl.cpp
	g++ -o procctl procctl.cpp
	cp procctl ../bin/.

checkproc:checkproc.cpp	
	g++ $(CFLAGS) -o checkproc checkproc.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp checkproc ../bin/.

gzipfiles:gzipfiles.cpp	
	g++ $(CFLAGS) -o gzipfiles gzipfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp gzipfiles ../bin/.

deletefiles:deletefiles.cpp
	g++ $(CFLAGS) -o deletefiles deletefiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp deletefiles ../bin/.

ftpclient_demo:ftpclient_demo.cpp
	g++ $(CFLAGS) -o ftpclient_demo ftpclient_demo.cpp $(PUBINCL) $(PUBCPP) /project/public/_ftp.cpp -L/project/public -lftp -lm -lc
	
ftpgetfiles:ftpgetfiles.cpp
	g++ $(CFLAGS) -o ftpgetfiles ftpgetfiles.cpp $(PUBINCL) $(PUBCPP) /project/public/_ftp.cpp -L/project/public -lftp -lm -lc
	cp ftpgetfiles ../bin/.

ftpputfiles:ftpputfiles.cpp
	g++ $(CFLAGS) -o ftpputfiles ftpputfiles.cpp $(PUBINCL) $(PUBCPP) /project/public/_ftp.cpp -L/project/public -lftp -lm -lc
	cp ftpputfiles ../bin/.

tcpputfiles:tcpputfiles.cpp
	g++ $(CFLAGS) -o tcpputfiles tcpputfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp tcpputfiles ../bin/.


tcpgetfiles:tcpgetfiles.cpp
	g++ $(CFLAGS) -o tcpgetfiles tcpgetfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp tcpgetfiles ../bin/.

fileserver:fileserver.cpp
	g++ $(CFLAGS) -o fileserver fileserver.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp fileserver ../bin/.

execsql:execsql.cpp
	g++ $(CFLAGS) -o execsql execsql.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp execsql ../bin/.


dminingmysql:dminingmysql.cpp
	g++ $(CFLAGS) -o dminingmysql dminingmysql.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp dminingmysql ../bin/.

xmltodb:xmltodb.cpp _tools.h _tools.cpp
	g++ $(CFLAGS) -o xmltodb xmltodb.cpp _tools.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp xmltodb ../bin/.


syncupdate:syncupdate.cpp _tools.h _tools.cpp
	g++ $(CFLAGS) -o syncupdate syncupdate.cpp _tools.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp syncupdate ../bin/.

clean:
	rm -f procctl checkproc gzipfiles deletefiles ftpclient_demo ftpgetfiles ftpputfiles tcpputfiles fileserver tcpgetfiles execsql\
	dminingmysql xmltodb syncupdate