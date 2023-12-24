#pragma once
#include "CTCPServer.h"
#include "CThreadPool.h"
#include "CChildTask.h"
#include <sys/epoll.h>
#include <iostream>
#include <string.h>
#include <map>
#include "protocol.h"
using namespace std;
#define EPOLLEVENT_NUM 5

class CTCPEpollServerModel
{
public:
	CTCPEpollServerModel(CTCPServer* TCPServer, CThreadPool* ThreadPool);
	void epollCtl(int fd, int option);//EPOLL_CTL_ADD    EPOLL_CTL_DEL
	void epollStart();
	static void* getFromPost(void* arg);//等待后置服务器发回消息
	static void* printfLogsThread(void* arg);//打印日志线程
	static void* heartBeatThread(void* arg);//心跳线程
private:
	CTCPServer* TCPServer; 
	CThreadPool* ThreadPool; 
	map<int, int> cilentMap;//客户端在线map  fd，业务数
	int epollfd;
	int epollwaitnum;
	int socketfd;
	int acceptfd;
	struct epoll_event epollevent;
	struct epoll_event epolleventArray[EPOLLEVENT_NUM];
	char buf[1500];//缓冲区
	int allFdNum;//有效链接数
	map<int, int> heartBeatMap;//fd,计次  次数为2 下线
};


