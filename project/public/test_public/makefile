# 开发框架头文件路径。
PUBINCL = -I/project/public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
PUBCPP = /project/public/_public.cpp

# 编译参数。
CFLAGS = -g 

all:demo1

demo1:demo1.cpp
	g++ $(CFLAGS) -o demo1 demo1.cpp $(PUBINCL) $(PUBCPP) -lm -lc