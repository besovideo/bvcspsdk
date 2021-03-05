# 最新版本BVCSP库 见https://github.com/besovideo/PUSimulator

## 项目说明
besovideo smarteye 协议库BVCSP，提供C接口，接口在BVCSP.h头文件中。  
功能主要包括： 

- 登录/下线。
- 命令交互。
- 通道数据传输（音视频、GPS、串口、文件等）。
- IM即时通讯。
## 编译说明
- windows 32 位库。注意依赖库的应用程序需要也是WIN32。
- BVCSP SDK是vs2010编译器编码，lib目录中已经携带c/c++运行环境库，您发布版本时需要注意携带。

## 目录说明
```
|-- include 头文件
|-- lib DLL文件和lib文件
|-- doc 使用文档
|-- demo + 示例工程
|        |-- BVCSPTest   简单的测试源码：认证、登录、命令。
|        |-- PUSimulator 设备端示例源码：认证、登录、通道（音视频、GPS）、命令。
|-- bin 存放demo编译生成的可执行文件
```
## 源码地址

gitee版本会滞后github.

gitee: https://gitee.com/besovideo/bvcspsdk.git  
github: https://github.com/besovideo/bvcspsdk.git

## 论坛支持
http://bbs.besovideo.com:8067/forum.php?mod=forumdisplay&fid=37&page=1
