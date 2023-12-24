#include "COfflineTask.h"
#include "protocol.h"
#include "IPC.h"
#include "MyDataBase.h"
#include "MyLog.h"
#include <iostream>
using namespace std;
COfflineTask::COfflineTask(char* data, int length):CBaseTask(data,length)
{
}

COfflineTask::~COfflineTask()
{
}

void COfflineTask::working()
{
	//cout << "做下线" << endl;
	IPC::getInstance()->ctlCount(5, 0);//+请求下线
	HEAD head = { OFFLINE,sizeof(BACKMSGT),0};//头
	BACKMSGT backmsgt = { 0 };//登录返回包
	memcpy(&backmsgt, this->data + sizeof(HEAD), this->length);//拷贝业务 
	int fd = 0;
	//拷贝fd
	memcpy(&fd, this->data + sizeof(HEAD) + this->length, sizeof(int));
	cout <<endl <<"┌------------------------用户下线-----------------------┐" << endl;

	cout << "下线：fd=" << fd << " id = " << backmsgt.user_id << endl;

	string info = "用户请求下线";
	MyLog::GetInstance()->writeLogToDb("请求下线", 0, backmsgt.user_id, info); //登录 接收 写入日志 
	//拼接sql 找到更新登录状态
	char sql[200] = { 0 };
	sprintf(sql, "UPDATE tbl_user SET user_state = 0 WHERE user_id = '%d';", backmsgt.user_id);
	int res = MyDataBase::GetInstance()->insertDelUpd(sql);//更新用户状态
	if (res == 0)
	{
		cout << "更新用户状态成功" << endl;
		backmsgt.flag = 0;
		info = "用户下线成功";
		IPC::getInstance()->ctlCount(2, 1);//-用户登入
	}
	else
	{
		cout << "更新用户状态失败" << endl;
		backmsgt.flag = 1;
		info = "用户下线失败";
	}
	
	cout << "验证完毕发回返回包" << endl;
	memcpy(data, &head, sizeof(HEAD));
	memcpy(data + sizeof(HEAD), &backmsgt, sizeof(BACKMSGT));
	memcpy(data + sizeof(HEAD) + sizeof(BACKMSGT), &fd, sizeof(int));
	IPC::getInstance()->sendToFront(this->data);

	
	MyLog::GetInstance()->writeLogToDb("请求下线", 1, backmsgt.user_id, info);//登录 发送 写入日志

}
