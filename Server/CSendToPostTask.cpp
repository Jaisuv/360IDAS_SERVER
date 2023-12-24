#include "CSendToPostTask.h"
#include "IPC.h"
#include "CRCCheck.h"
#include "CRCTool.h"

CSendToPostTask::CSendToPostTask(char* data, int fd, map<int, int>* cilentMap) :CBaseTask(data)
{
	this->fd = fd;
	this->cilentMap = cilentMap;
}

CSendToPostTask::~CSendToPostTask()
{
	//cout << "去后置任务走析构" << endl;
}

void CSendToPostTask::working()
{
	HEAD head = {0};
	
	memcpy(&head, this->data, sizeof(HEAD));
	memcpy(this->data+sizeof(HEAD)+head.length,&this->fd,sizeof(int));//最后带上 fd
	
	cout << "head.type=" << head.type << " head.length=" << head.length << " crcnum=" << head.CRCnum << endl;
	unsigned crcnum2=CRCcheck::GetInstance()->doCheck(this->data+ sizeof(HEAD) ,  head.length);
	this->iter = this->cilentMap->find(this->fd);
	if(head.type!=IMAGE)//若不是发图片 零碎文件没有返回包
		++this->iter->second;//对map值进行++

	cout << endl << "┌----------------------IPC发送后置----------------------┐" << endl;
	if (crcnum2 == head.CRCnum)
	{
		if (head.type == OFFLINE)
		{
			
			while (this->iter->second!=0) 
			{
				//cout << "任务还没完成，不允许下线" << endl;
				sleep(1);
			}
			//cout << "任务完成了，允许下线" << endl;
		}
		IPC::getInstance()->sendToPost(this->data);
		IPC::getInstance()->ctlCount(0,0);//接收包数+
		cout << "fd " << this->fd << "发送一个任务给后置type=" << head.type << endl;
	}
	else
	{
		cout << "CRC校验错误，客户端需重发" << endl;
		BACKMSGT backmag = { 0 };
		backmag.flag = 2;//CRC校验错误
		switch (head.type)
		{
		case LOGIN:
			head.type = LOGRESPOND;
			break;
		case REGISTER: 
			head.type = REGRESPOND; 
			break;
		case VIDEO:
			head.type =  VIDEORESPOND;
			break;
		case RECORD:
			head.type = RECORDRESPOND;
			break;
		case SEARCH:
			head.type =SEARCHRESPOND;
			break;
		case IMAGE:
			//cout << "发送图片CRC错误"<< endl;
			return;
		}
		head.length = sizeof(BACKMSGT);
		bzero(this->data, sizeof(this->data));
		memcpy(this->data, &head, sizeof(HEAD));
		memcpy(this->data + sizeof(HEAD), &backmag, sizeof(BACKMSGT));//最后带上 fd
		int res=write(this->fd,this->data,sizeof(HEAD)+head.length);
		//cout << " 要求客户端重发 res=" << res << endl;
	}
	
	
}
