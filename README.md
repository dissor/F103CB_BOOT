# F103CB_BOOT

# 功能
1. 检测程序更新
2. 解压 QuickLz 压缩包
3. 跳转应用程序

# 空间结构

|分区|起始地址| 大小|
|--|--|--|
|IAP|0x0800 0000|7K|
|user date|0x0800 1C00|1K|
|Code|0x0800 2000|(120 - x)K|
|IAP|0x0800 2000 + (120 - x)K|x|

x 在 APP 中获取。

# 压缩格式

等级3，流缓冲8K，开始解压保护，压缩顺序每包1024字节

# 参考手册

[http://www.quicklz.com/manual.html](http://www.quicklz.com/manual.html)


