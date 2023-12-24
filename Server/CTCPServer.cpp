#include "CTCPServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
#define BACKLOG 10
CTCPServer::CTCPServer(unsigned short port):CBaseSocket(port)
{
	//传入端口号
	this->hostAddress = new CHostAddress(port);
}
//连接
void CTCPServer::work()
{
	//解决address already is use
	//设置网络文件描述符 端口重用
	int opt_val = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (void*)(&opt_val), sizeof(opt_val));
	//先对结构体取地址 再强转
	if (bind(socketfd, this->hostAddress->getAddr(), this->hostAddress->getlen()) < 0)
	{
		perror("bind error:");
		exit(0);
	}
	//监听
	if (listen(socketfd, BACKLOG) < 0)
	{
		perror("listen error:");
		exit(0);
	}
	cout << "服务器开启成功" << endl;
}
//断开连接
void CTCPServer::socketDisConnect()
{
	if (this->socketfd == -1)
	{
		perror("socketfd error");
	}
	else
	{
		close(this->socketfd);
		cout << "服务器已下线" << endl;
	}
}