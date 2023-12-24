#include "CRegisterTask.h"
#include "protocol.h"
#include "IPC.h"
#include "MyLog.h"
#include "MyDataBase.h"
#include <iostream>
using namespace std;
CRegisterTask::CRegisterTask(char* data, int length):CBaseTask(data,length)
{

}

CRegisterTask::~CRegisterTask()
{
}

void CRegisterTask::working()
{
	IPC::getInstance()->ctlCount(4, 0);//+注册包 
	HEAD head = { 0 };//头
	REGT regt = { 0 };//注册业务体
	BACKMSGT regBack = { 0 };//注册返回包
	memcpy(&regt, this->data + sizeof(HEAD), this->length);//拷贝业务
	int fd = 0;
	//拷贝fd
	memcpy(&fd, this->data + sizeof(HEAD) + this->length, sizeof(int));
	cout <<endl<< "┌-----------------------新用户注册----------------------┐" << endl;
	cout << "注册：fd=" << fd << " id = " << regt.id << " name= "<< regt.name << " pwd=" << regt.pwd << endl;
	
	//拼接sql 插入用户
	char sql[200] = { 0 };
	sprintf(sql, "INSERT INTO tbl_user (user_account,user_pwd,user_name)  VALUES ('%s','%s','%s');", regt.id, regt.pwd, regt.name);
	//cout << sql << endl;
	//执行sql
	int res = MyDataBase::GetInstance()->insertDelUpd(sql);//插入新用户
	if (0 == res)
	{
		regBack.flag = 0;//成功
		cout << "注册新用户成功" << endl;
		string info = "发送个人结构体数据包：\n用户名："; 
		info += regt.name; 
		info += "\n密码："; 
		info += regt.pwd; 
		int userid = 0;
		sprintf(sql, "SELECT user_id  FROM tbl_user WHERE user_account='%s';", regt.id); 
		char** qres = NULL; 
		int col = 0, row = 0; 
		res = MyDataBase::GetInstance()->getData(sql, qres, row, col);//插入新用户 
		userid = atoi(qres[1]); 
		
		MyLog::GetInstance()->writeLogToDb("注册", 0, userid, info); //注册 接收 写入日志 
		MyLog::GetInstance()->writeLogToDb("注册", 1, userid, info); //注册 发送 写入日志
	}
	else
	{
		regBack.flag = 1;//失败
		cout << "注册新用户失败" << endl;
	}
	
	cout << "执行完毕 发回返回包" << endl;
	head.type = REGRESPOND;//注册返回
	head.length = sizeof(BACKMSGT);//登录返回包
	memcpy(data, &head, sizeof(HEAD));
	memcpy(data + sizeof(HEAD), &regBack, sizeof(BACKMSGT));
	memcpy(data + sizeof(HEAD)+ sizeof(BACKMSGT), &fd, sizeof(int));
	IPC::getInstance()->sendToFront(this->data);
	
	bzero(&regBack, sizeof(BACKMSGT));
	bzero(&head, sizeof(HEAD));
	bzero(&regt, sizeof(REGT));
}
