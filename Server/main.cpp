#include <iostream>
#include "CTCPServer.h"
#include "CTCPEpollServerModel.h"
#include "CThreadPool.h"
#include "IPC.h"
using namespace std;

int main()
{
	IPC::getInstance();//先走一次构造
	//端口号 0~65535 10000以下 系统默认  自定义应在10000以后
	CTCPServer* tcpServer=new CTCPServer(10001);
	CThreadPool* threadPool = new CThreadPool(8,16);

	CTCPEpollServerModel* epoll = new CTCPEpollServerModel(tcpServer, threadPool);
	
	epoll->epollStart();//开始执行业务
	return 0;
}