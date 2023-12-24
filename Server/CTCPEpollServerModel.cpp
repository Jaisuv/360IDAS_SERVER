#include "CTCPEpollServerModel.h"
#include "CSendToPostTask.h"
#include "CSendToClient.h"
#include "CHeartBeatTask.h"
#include "IPC.h"


CTCPEpollServerModel::CTCPEpollServerModel(CTCPServer* TCPServer, CThreadPool* ThreadPool)
{
	this->TCPServer = TCPServer;
	this->ThreadPool = ThreadPool;
	this->TCPServer->startConnect();//连接socket
	this->socketfd = this->TCPServer->getsocketfd();

	bzero(this->buf, sizeof(this->buf));
	this->acceptfd = 0;//上线客户端 文件描述符
	this->epollwaitnum = 0;//等待客户端数量
	this->epollfd = epoll_create(EPOLLEVENT_NUM);//数量与数组一直
	this->epollCtl(this->socketfd, EPOLL_CTL_ADD);//添加
	allFdNum = 0;
	//开启一个独立线程用于获得后置数据返回
	pthread_t id,id2,id3;
	pthread_create(&id, NULL, getFromPost, this);
	pthread_create(&id2, NULL, printfLogsThread, this);
	pthread_create(&id3, NULL, heartBeatThread, this);

}
//epoll控制
void CTCPEpollServerModel::epollCtl(int fd, int option)
{
	bzero(&epollevent, sizeof(epollevent));
	epollevent.data.fd = fd;//服务器fd
	epollevent.events = EPOLLIN;
	//fd添加保存到事件列表
	epoll_ctl(epollfd, option, fd, &epollevent);
}

//工作函数
void CTCPEpollServerModel::epollStart()
{
	int res = 0;
	HEAD head = { 0 };
	int len = 0;
	bool addtask = true;
	int i = 0;
	
	while (1)
	{
		//等待连接
		cout << "epoll wait......" << endl;
		//epoll_wait 等待事件发生 事件发生则从红黑树添加数据到数组
		//返回值：epollwaitnum 发生事件的fd个数
		epollwaitnum = epoll_wait(epollfd, epolleventArray, 100, -1);
		if (epollwaitnum < 0)
		{
			perror("epoll_wait error");
			exit(0);
		}
		for (int i = 0; i < epollwaitnum; i++)
		{
			//判断是不是服务器的文件描述符
			if (epolleventArray[i].data.fd == socketfd)
			{
				cout << "新客户端请求连接服务器...." << endl;
				//阻塞函数 ，等待客户端连接
				acceptfd = accept(epolleventArray[i].data.fd, NULL, NULL);
				cout << "客户端 " << acceptfd << " 已上线" << endl;
				this->epollCtl(acceptfd, EPOLL_CTL_ADD);//添加
				cout << "向map添加fd" << acceptfd << endl; 
				this->cilentMap.insert(make_pair(acceptfd,-1)); //没有0个业务处理中 
				++allFdNum;
			}
			else if (epolleventArray[i].events & EPOLLIN)//短路操作 &两侧都执行 有事件发生但不是socketfd
			{
				cout << "客户端事件" << endl;
				bzero(&head, sizeof(HEAD));
				bzero(buf,sizeof(buf));

				res = read(epolleventArray[i].data.fd, &head, sizeof(HEAD));//读头
				if (res > 0)//客户端读到头数据
				{
					len = 0;
					addtask = true;//运行添加任务
					//读协议体
					for(i=0;i<3;i++)
					{
						res = read(epolleventArray[i].data.fd, buf + sizeof(HEAD) + len, head.length - len);
						if (res < 0)
						{
							addtask = false;
							break;
						}
						len += res;
						if (len == head.length)
						{
							break;
						}
					}
					if (addtask)//若允许添加任务
					{
						
						memcpy(buf,&head,sizeof(HEAD));
						//cout << "收到res=" << res << " 添加一个任务 " << buf << endl;
						CBaseTask* task;
						if (head.type == HEARTBEAT)
							task = new CHeartBeatTask(buf, epolleventArray[i].data.fd, &this->heartBeatMap);
						else
							task = new CSendToPostTask(buf, epolleventArray[i].data.fd,&this->cilentMap);
						this->ThreadPool->pushTask(task);
						
					}			
				}
				else//客户端掉线
				{
					this->epollCtl(epolleventArray[i].data.fd, EPOLL_CTL_DEL);
					cout << "客户端 " << epolleventArray[i].data.fd << " 已掉线" << endl;
					close(epolleventArray[i].data.fd);//关闭客户端fd
					cout << "从map删除fd"<< epolleventArray[i].data.fd << endl;
					this->cilentMap.erase(epolleventArray[i].data.fd); //0个业务处理中 
					this->heartBeatMap.erase(epolleventArray[i].data.fd); 
					--allFdNum;
				}
			}
		}
	}
}

void* CTCPEpollServerModel::getFromPost(void* arg)
{
	cout << "开启独立线程 获取后置返回" << endl;
	pthread_t id = pthread_self();//获取线程自己的id
	bool endThread = false;
	//确保主线程与当前执行线程逻辑完全分离，当前执行线程执行结束后，id会自动释放
	//分离目的:为了声明这个线程不会阻塞主线程的逻辑,pthread_detach不会终止当前执行
	pthread_detach(id);

	CTCPEpollServerModel* epoll = (CTCPEpollServerModel*)arg;
	char data[1500] = { 0 };
	while (1)
	{
		IPC::getInstance()->getFromPost(data);
		CBaseTask* task = new CSendToClient(data,&epoll->cilentMap,&epoll->heartBeatMap);
		epoll->ThreadPool->pushTask(task);
		bzero(data, sizeof(data));
	}
	return nullptr;
}

void* CTCPEpollServerModel::printfLogsThread(void* arg)
{
	CTCPEpollServerModel* epoll = (CTCPEpollServerModel*)arg; 
	while(1)
	{
		sleep(10);
		IPC::getInstance()->printfLog(epoll->allFdNum);
	}
	return nullptr;
}

void* CTCPEpollServerModel::heartBeatThread(void* arg)
{
	CTCPEpollServerModel* epoll = (CTCPEpollServerModel*)arg;
	map<int, int>::iterator iter;
	
	while (1)
	{
		cout <<endl<< "@@@@@@@@@@@@@@@@@@@@@@@@ 心跳检测 @@@@@@@@@@@@@@@@@@@@@@@@@@" << endl; 
		for (iter = epoll->heartBeatMap.begin(); iter != epoll->heartBeatMap.end(); iter++)
		{
			//cout << "fd="<<iter->first << "计次++" << endl;
			
			++iter->second;
			cout <<"客户端 fd="<< iter->first << "计次" << iter->second << endl;
			if (iter->second == 3)
			{
				cout << "长时间无反应，强制下线 fd "<<iter->first << endl;
				epoll->epollCtl(iter->first, EPOLL_CTL_DEL);
				//cout << "客户端 " << iter->first << " 已掉线" << endl;
				close(iter->first);//关闭客户端fd
				//cout << "从map删除fd" << iter->first << endl;
				epoll->cilentMap.erase(iter->first); //0个业务处理中 
				epoll->heartBeatMap.erase(iter->first);
				--epoll->allFdNum; 
			}
		}
		cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
		sleep(10);
	}
	return nullptr;
}
