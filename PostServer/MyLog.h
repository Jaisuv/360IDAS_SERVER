#pragma once
#include "MyDataBase.h"
#include <string>
class MyLog
{
public:
	static MyLog* GetInstance();
	//log_type  0接收  1发送
	void writeLogToDb(string work_name,int log_type, int user_id, string log_info);//日志写入数据库
private:
	MyLog();
	~MyLog();//析构
	static MyLog* instance;//实例
};

