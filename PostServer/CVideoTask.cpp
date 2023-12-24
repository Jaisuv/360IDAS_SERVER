#include "CVideoTask.h"
#include "protocol.h"
#include "IPC.h"
#include "MyLog.h"
#include "MyDataBase.h"
#include <iostream>
using namespace std;
CVideoTask::CVideoTask(char* data, int length):CBaseTask (data,length)
{
}

CVideoTask::~CVideoTask()
{
}

void CVideoTask::working()
{
	IPC::getInstance()->ctlCount(6, 0);//+视频输入
	HEAD head = { 0 };//头
	VIDEOT videot = { 0 };//注册业务体
	BACKMSGT backMsg = { 0 };//注册返回包
	memcpy(&videot, this->data + sizeof(HEAD), this->length);//拷贝业务
	int fd = 0;
	//拷贝fd
	memcpy(&fd, this->data + sizeof(HEAD) + this->length, sizeof(int));
	//cout << "fd=" << fd << endl;
	//cout << "videot.framenum = " << videot.framenum << endl;
	//cout << "videot.video_cover = " << videot.video_cover << endl;
	//cout << "videot.video_path = " << videot.video_path << endl;
	//cout << "videot.user_id = " << videot.user_id << endl;
	//cout << "videot.equipment_id = " << videot.equipment_id << endl;
	//拼接sql'
	char sql[500] = { 0 };
	cout <<endl<< "┌----------------------上传视频-------------------------┐" << endl;
	cout << "上传视频：fd=" << fd << " 视频名：" << videot.video_name << " 总帧数：" << videot.framenum << endl;
	sprintf(sql, "INSERT INTO tbl_video (video_frame,video_name,video_cover,video_path,user_id,equipment_id) \
		VALUES(%d, '%s', '%s', '%s', %d, %d); ",
		videot.framenum, videot.video_name, videot.video_cover, videot.video_path, videot.user_id, videot.equipment_id);
	//cout << sql << endl;
	int res = MyDataBase::GetInstance()->insertDelUpd(sql);
	string info;//信息
	if (res == 0)
	{
		backMsg.flag = 0;//写入成功
		char duration[20] = { 0 };//时长
		int sec = videot.framenum / 30;//总秒数
		int min = sec / 60;//分钟
		sec -= min * 60;
		sprintf(duration, "%d：%02d", min, sec); //拼合时间
		info = "发送视频结构体数据包：\n设备id：";
		info += to_string(videot.equipment_id);
		info += "\n视频名称：";
		info += videot.video_name;
		info += "\n视频封面路径：";
		info += videot.video_cover;
		info += "\n视频路径：";
		info += videot.video_path;
		info += "\n视频时长：";
		info += duration;
		cout << "写入视频成功" << endl;
		
	}
	else
	{
		backMsg.flag = 1;//写入失败
		info = "写入视频失败";
		cout << "写入视频失败" << endl;
	}
		
	head.type = VIDEORESPOND;//视频返回
	head.length = sizeof(BACKMSGT);//返回包

	MyLog::GetInstance()->writeLogToDb("上传视频数据", 0, videot.user_id, info); //上传视频数据 接收 写入日志
	cout << "执行完毕 结果返回前置" << endl;
	memcpy(data, &head, sizeof(HEAD));
	memcpy(data + sizeof(HEAD), &backMsg, sizeof(BACKMSGT));
	memcpy(data + sizeof(HEAD) + sizeof(BACKMSGT), &fd, sizeof(int));
	IPC::getInstance()->sendToFront(this->data); //发回前置
	MyLog::GetInstance()->writeLogToDb("上传视频数据", 1, videot.user_id, info); //上传视频数据 发回 写入日志
	bzero(&backMsg, sizeof(BACKMSGT));
	bzero(&videot, sizeof(VIDEOT));
	bzero(&head, sizeof(HEAD));
}
