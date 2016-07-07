1、系统日志信息截图：

![](http://t3.qpic.cn/mblogpic/a4ac7c63cfbec4b9abf6/460)

通过设置NS_LOG环境变量，在不改变脚本和重新编译的情况下来增加日志的级别：
$ export NS_LOG=UdpEchoClientApplication=level_all

![](http://t3.qpic.cn/mblogpic/ff35ca522c451a77eefc/120)

![](http://t3.qpic.cn/mblogpic/97ac4dceb7bd9b6a264c/120)

![](http://t3.qpic.cn/mblogpic/80bd1b5b9bf7e37bc0a0/120)

![](http://t3.qpic.cn/mblogpic/8a3d15480346eae4e82e/120)

改变之后输出的系统日志：

![](http://t3.qpic.cn/mblogpic/e28343ec921779090bd6/120)

2、用wireshark打开.pcap文件

![](http://t3.qpic.cn/mblogpic/ef516ee365beffd86f2e/120)

3、PyViz截图


![](http://t3.qpic.cn/mblogpic/247608dcb82424a6394e/120)

![](http://t3.qpic.cn/mblogpic/a22ce7b7fa77b20e4106/120)

4、NS3安装过程
下载文件：
wget http://www.nsnam.org/release/ns-allinone-3.21.tar.bz2
解压：
tar -xjf ns-allinone-3.17.tar.bz2
编译：
cd ns-allinone-3.21
./build.py  
cd ns-3.21
./waf configure
./waf

