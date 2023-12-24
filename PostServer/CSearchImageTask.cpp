#include "CSearchImageTask.h"
#include "protocol.h"
#include "IPC.h"
#include "MyLog.h"
#include "MyDataBase.h"
#include <iostream>
using namespace std;
CSearchImageTask::CSearchImageTask(char* data, int length):CBaseTask(data,length)
{
}

CSearchImageTask::~CSearchImageTask()
{
}

void CSearchImageTask::working()
{
	//cout << "延时10秒做测试" << endl;
	//sleep(10);
	IPC::getInstance()->ctlCount(11, 0);//+查询图片
	HEAD head = {SERCHIMAGERESPOND,sizeof(IMAGESEARCHREST),0};//查询图片返回
	SEARCHT searcht = { 0 };//查询请求
	IMAGESAERCHT imagesearcht = { 0 };//单个图片
	IMAGESEARCHREST imageRest = { 0 };//所有图片信息
	memcpy(&searcht, this->data + sizeof(HEAD), this->length);//拷贝业务
	string info = "接收视频查询请求数据包：\n设备id："; 
	info += to_string(searcht.equipment_id); 
	info += "\n查询页码："; 
	info += to_string(searcht.currentPage);
	MyLog::GetInstance()->writeLogToDb("查询图片信息", 0, searcht.user_id, info); //接收 写入日志
	int fd = 0;
	//拷贝fd
	memcpy(&fd, this->data + sizeof(HEAD) + this->length, sizeof(int));
	cout <<endl<<"┌------------------------查询图片-----------------------┐" << endl;
	cout << "查询图片：" << endl;
	cout << "fd=" << fd << " user_id =" << searcht.user_id 
		<< " equipment_id=" << searcht.equipment_id << " currentPage=" << searcht.currentPage << endl;

	//拼接sql 找到图片信息
	char sql[200] = { 0 };

	sprintf(sql, "SELECT image_name,image_type,image_path FROM tbl_image\
		WHERE user_id=%d AND equipment_id=%d ORDER BY image_id DESC LIMIT 4 OFFSET %d;"
		, searcht.user_id, searcht.equipment_id, (searcht.currentPage - 1) * 4);
	//cout << sql << endl;
	//执行sql
	char** qres = nullptr;//总行 包括表头
	int row = 0, col = 0;
	int res = MyDataBase::GetInstance()->getData(sql, qres, row, col);//获取
	if (0 == res)
	{
		imageRest.image_num = row; 
		info = "发回图片查询结果数据包：\n设备id：";
		info += to_string(searcht.equipment_id);
		info += "\n查询页码：";
		info += to_string(searcht.currentPage);
		info += "\n查出视频数：";
		info += to_string(imageRest.image_num);
		MyLog::GetInstance()->writeLogToDb("查询图片信息", 1, searcht.user_id, info); //接收 写入日志
		if (row > 0)
		{
			//MyLog::GetInstance()->writeLogToDb("查询视频信息", 1, searcht.user_id, info); //登录 接收 写入日志
			bzero(&imagesearcht, sizeof(imagesearcht));
			cout << "查到图片数据" << imageRest.image_num << "条" << endl;
			for (int i = 1; i < row + 1; i++)
			{
				//填入信息
				//cout << "第" << i << "个图片--------------------" << endl;
				strcpy(imagesearcht.image_name, qres[i * col]);
				imagesearcht.image_type = atoi(qres[i * col + 1]);
				strcpy(imagesearcht.image_path, qres[i * col + 2]);
				imageRest.image_arr[i - 1] = imagesearcht;

			}
			for (int j = 0; j < imageRest.image_num; j++)
			{
				cout << "第" << j << "个图片--------------------" << endl;
				cout << "image_name= " << imageRest.image_arr[j].image_name << endl;
				//cout << "image_type= " << imageRest.image_arr[j].image_type << endl;
				//cout << "image_path= " << imageRest.image_arr[j].image_path << endl;
			}
			cout << "查图片成功做返回" << endl;
			memcpy(data, &head, sizeof(HEAD)); 
			memcpy(data + sizeof(HEAD), &imageRest, head.length); 
			memcpy(data + sizeof(HEAD) + head.length, &fd, sizeof(int)); 
			IPC::getInstance()->sendToFront(this->data); 
		}
		else
		{
			cout << "一条都没查到" << endl;
		}

	}
	else
	{
		cout << "CSerchVideoTask sql failed 查询视频语句出错" << endl;
	}
}
