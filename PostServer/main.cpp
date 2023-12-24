#include <iostream>
#include "IPC.h"
#include "protocol.h"
#include "CThreadPool.h"
#include "CChildTask.h"
#include "CLoginTask.h"
#include "CRegisterTask.h"
#include "CVideoTask.h"
#include "CRecordTask.h"
#include "CImageGetTask.h"
#include "CImageSpliceTask.h"
#include "CSerchVideoTask.h"
#include "CSearchImageTask.h"
#include "COfflineTask.h"
#include "CImage.h"

#include <stdio.h>
#include <sqlite3.h>
#include <fstream>
using namespace std;

void* write_log(void* arg);//写日志线程函数
int main()
{
	////创建一个线程用于接收前置服务器发送的消息
	//开启写日志线程
	pthread_t id; 
	pthread_create(&id, NULL, write_log, NULL);

	map<int, CImage*> useridMap;//最外层map
	CThreadPool* threadpool = new CThreadPool(8, 16);
	char data[1000] = { 0 };
	CBaseTask* task =NULL;
	HEAD head = { 0 };
	while (1)
	{
		IPC::getInstance()->getFromFront(data);
		cout << "收到一个任务" << endl;
		memcpy(&head, data, sizeof(HEAD));
		switch (head.type)
		{
		case LOGIN:
			cout << "任务是登录" << endl;
			task = new CLoginTask(data,head.length);
			break;
		case REGISTER:
			cout << "任务是注册" << endl;
			task = new CRegisterTask(data, head.length);
			break;
		case VIDEO: 
			cout << "任务是上传视频数据" << endl;
			task = new CVideoTask(data, head.length);
			break;
		case RECORD:
			cout << "任务是上传播放记录" << endl;
			task = new CRecordTask(data, head.length);
			break;
		case SEARCH:
			cout << "任务是查询视频信息" << endl;
			task = new CSerchVideoTask(data, head.length);
			break;
		case IMAGE:
			cout << "任务是上传图片" << endl;
			task = new CImageGetTask(data, head.length, &useridMap);
			break;
		case IMAGECOMPLETE: 
			cout << "任务是拼合图片" << endl;
			task = new CImageSpliceTask(data, head.length, &useridMap);
			break;
		case SERCHIMAGE:
			cout << "任务是查询图片信息" << endl;
			task = new CSearchImageTask(data, head.length);
			break;
		case OFFLINE:
			cout << "任务是客户端安全下线" << endl;
			task = new COfflineTask(data, head.length);
			break;
		default :
			cout <<"无法识别的任务" << endl;
			task = new CChildTask(data, head.length);
			break;
		}
		threadpool->pushTask(task);
		bzero(&head, sizeof(HEAD));
		bzero(data,sizeof(data));
	}
	
	return 0;
}

//写日志线程函数
void* write_log(void* arg)
{
	time_t nowtime;
	struct tm* p;
	int preday = 0;
	int year = 0,mon=0,day=0,hour=0,min=0,second=0;//事件
	int res = 0;
	char dirName[20] = {0};
	char timeArr[20] = { 0 };//时间
	char fileName[20] = { 0 };//日志文件名
	char filePath[40] = { 0 };//日志文件完整路径
	char sql[200] = { 0 };
	char** qres = nullptr;//总行 包括表头
	int row = 0, col = 0;
	ofstream outfile;//输出文件流
	if (access("./logs", F_OK) == -1)
	{
		res = mkdir("./logs", 0777);
		cout << "创建./logs文件夹 res=" << res << endl;
	}
	while (1)
	{
		time(&nowtime); //获取1970年1月1日0点0分0秒到现在经过的秒数 
		p = localtime(&nowtime); //将秒数转换为本地时间,年从1900算起,需要+1900,月为0-11,所以要+1 
		year = p->tm_year + 1900;
		mon = p->tm_mon + 1; 
		day = p->tm_mday;
		hour = p->tm_hour;
		min = p->tm_min;
		second = p->tm_sec;
		if (preday != day)//天数对不上，新一天到了 新建文件夹
		{
			//重组文件夹名
			bzero(dirName, sizeof(dirName));
			sprintf(dirName,"./logs/%04d-%02d-%02d", year, mon, day);
			if (access(dirName, F_OK) == -1)
			{
				res = mkdir(dirName, 0777);
				cout << "创建"<< dirName <<"文件夹 res=" << res << endl;
			}
		}
		//重组时间
		bzero(timeArr, sizeof(timeArr));
		sprintf(timeArr, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, second);
		//重组文件名
		bzero(fileName, sizeof(fileName));
		sprintf(fileName, "log%04d-%02d-%02d %02d:%02d:%02d .txt", year, mon, day, hour, min, second);
		//重组完整文件路径
		bzero(filePath, sizeof(filePath));
		sprintf(filePath, "%s/%s", dirName, fileName);

		sleep(20);//延时时间

		
		//拼接sql 找到用户名称
		char sql[200] = { 0 };
		sprintf(sql, "SELECT  a.log_time, b.work_name, a.log_type, a.user_id, a.log_info\
			FROM tbl_log a\
			LEFT JOIN tbl_work b\
			ON a.work_id = b.work_id\
			WHERE log_time > '%s';", timeArr);
		//cout << sql << endl;
		//执行sql
		
		cout <<endl<< "读取日志写入文件" << endl;
		res = MyDataBase::GetInstance()->getData(sql, qres, row, col);//获取
		if (0 == res)
		{
			if (row > 0)
			{
				cout << "查到数据往文件写内容" << endl;
				outfile.open(filePath,ios::out);//打开文件流
				for (int i = 1; i < row + 1; i++)
				{
					outfile << "时间：" << qres[i * col] << endl;
					outfile << "功能：" << qres[i * col+1] << endl;
					if (atoi(qres[i * col + 2]) == 0)
					{
						outfile << "类型：接收" << endl;
					}
					else
					{
						outfile << "类型：发送" << endl;
					}
					outfile << "用户id：" << qres[i * col+3] << endl;
					outfile << "时间：" << qres[i * col+4] << endl << endl;
				}
				outfile.close();
			}
			else
			{
				cout << "查不到数据，不写" << endl;
			}

		}
		else
		{
			cout << "写日志sql错误" << endl;
		}
		
	}
	
	return NULL;
}