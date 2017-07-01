# VSSFTP
VSSFTP是visual studio 2015的sftp扩展插件,主要用于直接在VS中上传,下载代码

## 1, 安装插件
编译完成后,双击VSSFTP\VSSFTPVSIX\bin\Debug\VSSFTP.vsix,进行安装

## 2, 配置文件
在项目解决方案文件(***.sln)同目录下,添加文件ssh.xml
![alt text](VSSFTP/ssh.xml "ssh.xml")

## 3, 上传文件

在VS的"解决方案资源管理器"中选中需要上传文件,右击弹出菜单,
菜单中选择"上传文件(U)"开始上传文件

在完成后会在任务栏中显示完成结果
> 从(123.123.123.123:22)上传文件完成(2/2)

在"输出"窗口内会显示详细的上传信息
> Starting zmodem transfer.Press Ctrl + C to cancel.
> Transferring /data/test/makefile
>   100%	2471 B	2471 B/s	1 Second	0 Errors

## 4, 下载文件

在VS的"解决方案资源管理器"中选中需要下载文件,右击弹出菜单,
菜单中选择"下载文件(D)"开始下载文件

在完成后会在任务栏中显示完成结果
> 从(123.123.123.123:22)下载文件完成(2/2)

在"输出"窗口内会显示详细的下载文件信息
> Starting zmodem transfer.Press Ctrl + C to cancel.
> Transferring E:\Work\test\makefile
>   100%	2471 B	2471 B/s	1 Second	0 Errors
