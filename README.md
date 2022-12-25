# EasyConMCU_CH32

EasyConMCU_CH32，目前最便宜，功能也是最完整的方案，不需要额外购买杜邦线、也不用焊接、也不需要串口。



## CH32F103C8T6双USB

目前这个开发板只有一家店，也是所有方案里目前最便宜的

> https://item.taobao.com/item.htm?spm=a1z09.2.0.0.780d2e8dz0P9f7&id=676716722836&_u=p1g76mj93bf5



![image-20221216220451721](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212162204792.png)

- Type-C和micro-usb都可以用，看你具体哪种线比较多



#### 注意

由于两个usb口过于接近，为了能同时插入两个USB，可能需要你把usb的柄削薄一些（注意不要过于用力，小心手）

![image-20221225182417384](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212251824558.png)

![image-20221225182423908](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212251824005.png)

当然也可以直接买一根类似这种超薄的线，握柄厚度小于等于3mm的

![image-20221225182906203](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212251829244.png)



## 准备

从群文件中下载

- CH372串口驱动
- 烧写程序WCHISPTool_Setup

分别解压并安装

板子断电，然后将BOOT0接1，Boot1接0，如图所示

![image-20221225184444921](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212251844048.png)

标记有HUSB的就是串口、也是烧写口，此时连接到HSUB到PC

![image-20221225184550579](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212251845628.png)



## 烧写

下载最新的固件，一般在伊机控Firmware文件夹中

打开烧写软件WCHISPTool(V2.9)

![image-20221225184100896](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212251841941.png)

1. 选择CH32F1系列
2. 选择USB
3. 选择CH32F103
4. 确保读保护是被取消了
5. 选择伊机控中的最新固件
6. 点击下载



断开USB，恢复boot，BOOT0接0，Boot1接0，如图所示

![image-20221225185530391](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212251855488.png)



## 连接伊机控

HUSB连接PC，另外一个USB连接NS

关闭刚才的烧写软件，点击自动连接，显示已连接说明成功，可以使用了。

![image-20221205224440983](http://img.elmagnifico.tech:9514/static/upload/elmagnifico/202212052244036.png)

- 第一次使用可能需要，远程停止，并且清除烧录一次，后续就正常了



## 常见问题

伊机控连接上了，NS没有反应，重启NS
