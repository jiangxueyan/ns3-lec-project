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
