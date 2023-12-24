#include "CSerchVideoTask.h"
#include "protocol.h"
#include "IPC.h"
#include "MyLog.h"
#include "MyDataBase.h"
#include <iostream>
using namespace std;
CSerchVideoTask::CSerchVideoTask(char* data, int length) :CBaseTask(data, length)
{
}

CSerchVideoTask::~CSerchVideoTask()
{
}

void CSerchVideoTask::working()
{
	IPC::getInstance()->ctlCount(8, 0);//+视频数据
	HEAD head = {SEARCHRESPOND,sizeof(SEARCHREST),0};//头
	SEARCHT searcht = { 0 };//业务
	VIDEOT videot = { 0 };//视频具体信息头
	SEARCHREST searchrest = { 0 };//视频查询返回包 具体包
	memcpy(&searcht, this->data + sizeof(HEAD), this->length);//拷贝业务
	int fd = 0;
	//拷贝fd
	memcpy(&fd, this->data + sizeof(HEAD) + this->length, sizeof(int));
	cout<<endl << "┌-----------------------查询视频------------------------┐" << endl;
	cout << "查询视频：" << endl;
	cout << "fd=" << fd << " user_id =" << searcht.user_id 
		<< " equipment_id=" << searcht.equipment_id << " currentPage=" << searcht.currentPage << endl;
	string info = "接收视频查询请求数据包：\n设备id："; 
	info += to_string(searcht.equipment_id);
	info += "\n查询页码：";
	info += to_string(searcht.currentPage); 
	MyLog::GetInstance()->writeLogToDb("查询视频信息", 0, searcht.user_id, info); //接收 写入日志

	//拼接sql 找到视频信息以及记录
	char sql[500] = { 0 };
	sprintf(sql, "SELECT  a.video_frame, b.record_frame, a.video_name, a.video_cover, a.video_path\
		FROM  tbl_video a\
		LEFT JOIN  tbl_record b\
		ON a.video_id = b.video_id AND b.user_id = %d\
		WHERE a.equipment_id = %d ORDER BY a.video_id DESC LIMIT 4 OFFSET %d; "
		, searcht.user_id, searcht.equipment_id, (searcht.currentPage-1)*4); 
	//cout << sql << endl;
	//执行sql
	char** qres = nullptr;//总行 包括表头
	int row = 0, col = 0;
	int res = MyDataBase::GetInstance()->getData(sql, qres, row, col);//获取
	if (0 == res)
	{
		if (row > 0)
		{
			searchrest.num = row;
			info = "发回视频查询结果数据包：\n设备id：";
			info += to_string(searcht.equipment_id);
			info += "\n查询页码：";
			info += to_string(searcht.currentPage);
			info += "\n查出视频数：";
			info += to_string(searchrest.num);
			MyLog::GetInstance()->writeLogToDb("查询视频信息", 1, searcht.user_id, info); //接收 写入日志
			bzero(&videot,sizeof(videot));
			cout << "查到视频" << searchrest.num<<"条" << endl;
			for (int i = 1; i < row+1; i++)
			{
				//填入信息
				//cout << "第" <<i<< "个视频--------------------" << endl;
				//cout << "framenum="<< atoi(qres[i*col]) << endl;
				videot.framenum = atoi(qres[i * col]);
				if (qres[i * col + 1] == NULL)
				{
					//cout << "currentnum= 0"  << endl;
					videot.currentframe = 0;
				}
				else
				{
					//cout << "currentnum= " << atoi(qres[i * col+1]) << endl;
					videot.currentframe = atoi(qres[i * col + 1]);
				}
				strcpy(videot.video_name, qres[i * col + 2]);
				strcpy(videot.video_cover, qres[i * col + 3]);
				strcpy(videot.video_path, qres[i * col + 4]);
				searchrest.videoarr[i-1] = videot;
				//cout << "videoname=" << qres[i * col + 2] << endl;
				//cout << "videocover=" << qres[i * col + 3] << endl;
				//cout << "videopath=" << qres[i * col + 4] << endl;
			}
			for (int j = 0; j < searchrest.num; j++)
			{
				cout << "查到第" << j << "个视频--------------------" << endl;
				//cout <<"framenum= " << searchrest.videoarr[j].framenum << endl;
				//cout << "currentframe= " << searchrest.videoarr[j].currentframe << endl;
				cout << "video_name= " << searchrest.videoarr[j].video_name << endl;
				//cout << "video_cover= " << searchrest.videoarr[j].video_cover << endl;
				//cout << "video_path= " << searchrest.videoarr[j].video_path << endl;
			}
			
		}
		else
		{
			searchrest.num = row; 
			cout << "一条都没查到" << endl;
		}

		cout << "查视频成功做返回" << endl;
		memcpy(data, &head, sizeof(HEAD));
		memcpy(data + sizeof(HEAD), &searchrest, head.length);
		memcpy(data + sizeof(HEAD) + head.length, &fd, sizeof(int));
		IPC::getInstance()->sendToFront(this->data);
	}
	else
	{
		cout << "CSerchVideoTask sql failed 查询视频语句出错" << endl;
	}
	
}
