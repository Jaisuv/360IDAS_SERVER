#include "CLoginTask.h"
#include "protocol.h"
#include "IPC.h"
#include "MyDataBase.h"
#include "MyLog.h"
#include <iostream>
using namespace std;
CLoginTask::CLoginTask(char* data, int length):CBaseTask(data,length)
{

}

CLoginTask::~CLoginTask()
{
}

void CLoginTask::working()
{
	IPC::getInstance()->ctlCount(3, 0);//+请求登录
	//业务
	HEAD head = { 0 };//头
	LOGT logt = {0};
	BACKMSGT logrespond = { 0 };//登录返回包
	memcpy(&logt, this->data + sizeof(HEAD), this->length);//拷贝业务
	int fd = 0;
	//拷贝fd
	memcpy(&fd, this->data + sizeof(HEAD)+this->length, sizeof(int));
	cout <<endl<< "┌------------------------用户登录-----------------------┐" << endl;
	cout <<"登录 ： fd=" << fd << " id = " << logt.id << " pwd=" << logt.pwd << endl;
	
	//拼接sql 找到用户名称
	char sql[200] = { 0 };
	sprintf(sql,"SELECT user_name,user_id FROM tbl_user WHERE user_account ='%s' AND user_pwd ='%s';", logt.id,logt.pwd);
	//cout << sql << endl;
	//执行sql
	char** qres = nullptr;//总行 包括表头
	int row = 0, col = 0;
	int res = MyDataBase::GetInstance()->getData(sql, qres, row, col);//获取
	if (0 == res)
	{
		if (row > 0)
		{
			
			
			logrespond.flag = 0;
			strcpy(logrespond.name, qres[2]);
			logrespond.user_id = atoi(qres[3]);
			cout << "检索成功,查到用户"<< qres[2] <<" logrespond.user_id="<< logrespond.user_id << endl;
			sprintf(sql, "UPDATE tbl_user SET user_state = 1 WHERE user_id = '%d';", logrespond.user_id);
			res = MyDataBase::GetInstance()->insertDelUpd(sql);//更新用户状态
			IPC::getInstance()->ctlCount(2,0);//+用户登入
			if (res == 0)
				cout << "更新用户状态成功" << endl;
			else
				cout << "更新用户状态失败" << endl;
		}
		else
		{
			logrespond.flag = 1;
			cout << "检索失败，无此用户" << endl;
		}

	}
	else
	{
		cout << "CLoginTask sql failed 登录语句出错--51" << endl;
	}
	string info = "发送个人结构体数据包：\n用户名：";
	info += logt.id;
	info += "\n密码：";
	info += logt.pwd;
	MyLog::GetInstance()->writeLogToDb("登录",0, logrespond.user_id,info); //登录 接收 写入日志

	cout << "验证完毕发回返回包" << endl;
	head.type = LOGRESPOND;//返回
	head.length = sizeof(BACKMSGT);//登录返回包
	memcpy(data, &head, sizeof(HEAD));
	memcpy(data + sizeof(HEAD), &logrespond, sizeof(BACKMSGT));
	memcpy(data + sizeof(HEAD)+ sizeof(BACKMSGT), &fd, sizeof(int));
	IPC::getInstance()->sendToFront(this->data);

	MyLog::GetInstance()->writeLogToDb("登录", 1, logrespond.user_id, info);//登录 发送 写入日志
	bzero(&logt, sizeof(LOGT));
	bzero(&head, sizeof(HEAD));
	bzero(&logrespond, sizeof(BACKMSGT));
}
