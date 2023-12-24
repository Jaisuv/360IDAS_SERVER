#include "CSendToClient.h"
#include "protocol.h"
#include <iostream>
#include <unistd.h>
#include "IPC.h"
using namespace std;
CSendToClient::CSendToClient(char* data, map<int, int>* cilentMap, map<int, int>* heartBeatMap):CBaseTask(data)
{
	this->cilentMap = cilentMap;
	this->heartBeatMap = heartBeatMap; 
}

CSendToClient::~CSendToClient()
{
	//cout << "发回客户端任务走析构" << endl;
}

void CSendToClient::working()
{
	HEAD head = {0};
	memcpy(&head, this->data, sizeof(HEAD));//后置发给前置 协议头
	int fd = 0;//文件描述符
	if (head.type == LOGRESPOND)
	{
		BACKMSGT backmsg = { 0 };
		memcpy(&backmsg, data+sizeof(HEAD), head.length);
		if (backmsg.flag==0)
		{
			this->heartBeatMap->insert(make_pair(fd, 0));
		}  
	}
	memcpy(&fd,this->data+sizeof(HEAD)+head.length,sizeof(int));

	int res = write(fd, this->data, sizeof(HEAD) +head.length);//发送响应

	cout << endl << "┌-------------------------反包--------------------------┐" << endl;
	cout << " 拼合返回包 发回客户端 res=" << res << endl;
	--this->cilentMap->find(fd)->second;
	IPC::getInstance()->ctlCount(1, 0);//发送包数+
	//cout << "从map里减业务数，业务数" << this->cilentMap->find(fd)->second<< endl;
}
