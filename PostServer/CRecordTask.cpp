#include "CRecordTask.h"
#include "protocol.h"
#include "IPC.h"
#include "MyLog.h"
#include "MyDataBase.h"
#include <iostream>
using namespace std;
CRecordTask::CRecordTask(char* data, int length):CBaseTask(data,length)
{
}

CRecordTask::~CRecordTask()
{
}

void CRecordTask::working()
{
	IPC::getInstance()->ctlCount(7, 0);//+视频记录
	HEAD head = { 0 };
	VIDEOT videot = { 0 };
	BACKMSGT backMsg = { 0 };
	memcpy(&videot, this->data + sizeof(HEAD), this->length);//拷贝业务
	int fd = 0;
	//拷贝fd
	memcpy(&fd, this->data + sizeof(HEAD) + this->length, sizeof(int));
	cout <<endl<< "┌---------------------上传播放记录----------------------┐" << endl;
	cout << "播放记录上传： fd=" << fd << endl;
	cout << "视频名：" << videot.video_name << endl;
	cout << "观看帧数：" << videot.currentframe << endl;
	//cout << "fd=" << fd << endl;
	//cout << "videot.currentframe = " << videot.currentframe << endl;
	//cout << "videot.user_id = " << videot.user_id << endl;
	//cout << "videot.video_name = " << videot.video_name << endl;
	//cout << "videot.equipment_id = " << videot.equipment_id << endl;


	//拼接sql 插入用户
	char sql[200] = { 0 };
	
	sprintf(sql, "SELECT record_id FROM tbl_record WHERE video_id = \
		(SELECT video_id FROM tbl_video WHERE video_name = '%s' AND equipment_id = %d)\
		AND user_id = %d AND equipment_id = %d;",
		videot.video_name, videot.equipment_id, videot.user_id,videot.equipment_id);
	//执行sql
	char** qres = nullptr;//总行 包括表头
	int row = 0, col = 0;
	int res = MyDataBase::GetInstance()->getData(sql, qres, row, col);//获取 
	if (0 == res)//若语句无误
	{
		if (row > 0)//若找到做更新
		{
			cout << "检索成功,查到记录做更新" << qres[1] << endl;
			int record_id = atoi(qres[1]);
			cout << record_id << endl;
			sprintf(sql, "UPDATE tbl_record SET record_frame = %d WHERE record_id = %d;",videot.currentframe, record_id);
			//cout << sql << endl;
		}
		else
		{
			cout << "没有这条记录做插入" << endl;
			sprintf(sql, "INSERT INTO tbl_record(record_frame, user_id, video_id, equipment_id)\
			VALUES(%d, %d, (SELECT video_id FROM tbl_video WHERE video_name = '%s' AND equipment_id = %d), %d);", 
				videot.currentframe, videot.user_id, videot.video_name, videot.equipment_id, videot.equipment_id); 
			//cout << sql << endl;
		}

	}
	else//语句错误
	{
		cout << "CRecprdTask sql failed 视频记录sql语句出错--61" << endl;
		return;
	}
	//执行sql
	res = MyDataBase::GetInstance()->insertDelUpd(sql);//插入或更新 播放记录 
	if (0 == res)
	{
		backMsg.flag = 0;//成功
		cout << "更新新纪录成功" << endl;
		char duration[20] = { 0 };//时长
		int sec = videot.currentframe / 30;//总秒数  
		int min = sec / 60;//分钟
		sec -= min * 60;
		sprintf(duration, "%d：%02d", min, sec); //拼合时间
		string info = "发送视频记录结构体数据包：\n设备id：";
		info += to_string(videot.equipment_id);
		info += "\n视频名称：";
		info += videot.video_name;
		info += "\n观看时长：";
		info += duration;
		cout << "写入视频记录成功" << endl;

		
		MyLog::GetInstance()->writeLogToDb("上传播放记录", 0, videot.user_id, info); //注册 接收 写入日志 
		MyLog::GetInstance()->writeLogToDb("上传播放记录", 1, videot.user_id, info); //注册 发送 写入日志
	}
	else
	{
		backMsg.flag = 1;//失败
		cout << "更新新记录失败" << endl;
	}
	cout << "执行完毕 发回返回包" << endl;
	head.type = RECORDRESPOND;//记录返回
	head.length = sizeof(BACKMSGT);//登录返回包
	memcpy(data, &head, sizeof(HEAD));
	memcpy(data + sizeof(HEAD), &backMsg, sizeof(BACKMSGT));
	memcpy(data + sizeof(HEAD) + sizeof(BACKMSGT), &fd, sizeof(int));
	IPC::getInstance()->sendToFront(this->data);

}
