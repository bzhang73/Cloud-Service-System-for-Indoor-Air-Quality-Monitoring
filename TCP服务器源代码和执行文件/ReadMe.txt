tcps.c 是与硬件建立TCP连接的服务器端,可以接收客户端传来的数据,并对数据进行分析,将符合要求的数据,通过mongodb-c-drive直接写入数据库
test 由tcps.c通过如下命令编译生成的可执行文件,在ubuntu 14.04系统下,使用命令./test来执行文件

gcc -o test tcps.c -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0/ -lmongoc-1.0 -lbson-1.0
