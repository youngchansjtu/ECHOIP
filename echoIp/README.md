本项目功能：通过nslookup返回客户端自身ip，该代码兼容mac平台和windows平台(两个文件夹代码完全一致)
使用方法：
	１.项目编译完成后生成　echoIp　可执行文件(先执行cmake_make.sh,再在build文件夹下make)
	２.启动服务器方式有两种
		A 指定端口　　        ./echoIp 端口号
		B 使用默认端口５３　　　./echoIp
		鉴于windows下的nslookup工具不支持指定端口，建议采用第二种方式启动服务器
	３.客户端获取自身ip
　		先执行nslookup，会进入交互模式，再执行如下指令：
		server 服务器ip        指定服务器
		set port=端口id        指定服务器端口（不指定时，使用默认端口５３）
		此时就可以进行ip查询了，输入任意域名均会返回自身ip