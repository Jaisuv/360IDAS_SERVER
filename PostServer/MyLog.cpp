#include "MyLog.h"
#include "MyDataBase.h"
#include<ctime>
#include<stdio.h>
MyLog* MyLog::instance = new MyLog();
MyLog* MyLog::GetInstance()
{
    return nullptr;
}

void MyLog::writeLogToDb(string work_name, int log_type, int user_id, string log_info)
{
	char logTime[20] = { 0 };
	time_t nowtime;
	time(&nowtime); //获取1970年1月1日0点0分0秒到现在经过的秒数 
	tm* p = localtime(&nowtime); //将秒数转换为本地时间,年从1900算起,需要+1900,月为0-11,所以要+1 
	sprintf(logTime,"%04d-%02d-%02d %02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

	//拼接sql 找到用户名称
	char sql[500] = { 0 };
	int workid = -1;
	//执行sql
	char** qres = nullptr;//总行 包括表头
	int row = 0, col = 0;
	//查业务类型id
	sprintf(sql, "SELECT work_id FROM tbl_work WHERE work_name='%s';", work_name.c_str());
	int res = MyDataBase::GetInstance()->getData(sql, qres, row, col);//获取
	if (0 == res)
	{
		if (row > 0)
		{
			workid = atoi(qres[1]);
			//cout << "获取到业务功能id" << workid << endl;
		}
	}
	//拼接sql 插入新日志
	sprintf(sql, "INSERT INTO tbl_log(log_time, work_id, log_type, user_id, log_info) "
		"VALUES('%s',%d, %d, %d, '%s');", logTime, workid, log_type, user_id, log_info.c_str());
	//cout << sql << endl;
	//执行sql
	res = MyDataBase::GetInstance()->insertDelUpd(sql);//插入新日志
	cout << "---插入了日志记录---" << endl;
	/*cout << "时间：" << logTime<< endl;
	cout << "功能：" << workid << endl;
	cout << "类型：" << log_type << endl; 
	cout << "用户 id：" << userid << endl;
	cout << log_info << endl<<endl;*/
}


MyLog::MyLog()
{
   
}

MyLog::~MyLog()
{
}
