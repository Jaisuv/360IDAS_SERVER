#include "CImageSpliceTask.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "IPC.h"
#include "MyLog.h"
CImageSpliceTask::CImageSpliceTask(char* data, int length, map<int, CImage*>* useridMap) :CBaseTask(data, length)
{
	this->useridMap = useridMap;
}

CImageSpliceTask::~CImageSpliceTask()
{
}

void CImageSpliceTask::working()
{
	IPC::getInstance()->ctlCount(10, 0);//+拼合文件
	IMAGEOKT imageokt = { 0 };
	memcpy(&imageokt, this->data + sizeof(HEAD), this->length);
	
	//cout<<" imageokt.user_id= " << imageokt.user_id << " imageokt.img_num=" 
	//	<< imageokt.img_name << " imageokt.img_type=" << imageokt.img_type << endl;
	map<int, CImage*>::iterator iter; //最外层 useridmap 迭代器
	int userid = imageokt.user_id;//用户id临时变量
	iter = useridMap->find(imageokt.user_id); 
	cout <<endl <<"┌-----------------------拼合图片------------------------┐" << endl;
	cout << "零碎文件数目" << iter->second->getImg_dataMap().size()<<endl;

	
	//拼合协议
	HEAD head = { IMAGERESPOND,sizeof(IMAGEBACKT),0 };
	IMAGEBACKT imageback = { 0 };//返回包
	char buf[1500] = { 0 };
	int fd = 0;
	memcpy(&fd, this->data + sizeof(HEAD) + this->length, sizeof(int));//读fd
	memcpy(buf, &head, sizeof(HEAD));
	memcpy(buf + sizeof(HEAD) + head.length, &fd, sizeof(int));
	
	if (iter->second->compareBytesAndNum())
	{
		cout << "校验通过" << endl;
		umask(0);
		int res = 0;
		if (access("./image", F_OK) == -1)
		{
			res = mkdir("./image", 0777);
			cout << "创建./image文件夹 res=" <<res<< endl;
		}
		string imagepath = "./image/";
		imagepath += iter->second->getEquipment_id();//设备id
		if (access(imagepath.c_str(), F_OK) == -1)
		{
			res = mkdir(imagepath.c_str(), 0777);
			cout << "创建" << imagepath << " res=" << res << endl;
		}
		imagepath += "/";
		imagepath += to_string(imageokt.user_id);

		string imagepath2;
		//找文件夹 创建文件夹
		if (access(imagepath.c_str(), F_OK) == -1)//若用户文件夹都找不到 就创建所有文件夹
		{
			res = mkdir(imagepath.c_str(), 0777);//创建用户
			cout << "创建" << imagepath << " res=" << res << endl;
			//行车
			imagepath2 = imagepath;
			imagepath2+= "/driving";
			res=mkdir(imagepath2.c_str(), 0777);
			cout << "创建" << imagepath2 << " res=" << res << endl;
			//红绿灯
			/*imagepath2 = imagepath;
			imagepath2 += "/trafficlight";
			res = mkdir(imagepath2.c_str(), 0777);
			cout << "创建" << imagepath2 << " res=" << res << endl;*/
			//手动
			imagepath2 = imagepath; 
			imagepath2 += "/manual";
			res = mkdir(imagepath2.c_str(), 0777);
			cout << "创建" << imagepath2 << " res=" << res << endl;
			
		}
		// 根据文件类型 确定文件要保存到的路径
		imagepath2 = imagepath;
		switch (imageokt.img_type)
		{
		case 0:
			imagepath2 += "/driving";
			break;
		default:
			imagepath2 += "/manual";
			break;
		}
		imagepath2 += "/";
		imagepath2 += iter->second->getImg_name();
		cout << "imagepath=" << imagepath2 << endl;

		int endbyte = iter->second->getAllBytes()%1000;
		int nums = iter->second->getAllnums();
		int i = 1;
		cout << "filepath=" << imagepath2 <<" endbyte="<< endbyte << endl;
		map<int, char *>::iterator iter2;//文件数据map迭代器
		int wfd = open(imagepath2.c_str(), O_CREAT | O_WRONLY, 0777);
		
		for (iter2 = iter->second->getImg_dataMap().begin(); iter2 != iter->second->getImg_dataMap().end(); iter2++)
		{
			if (i == nums)
			{
				res=write(wfd,iter2->second, endbyte);
				cout << "key=" << iter2->first << " 拼完了关文件" << i<<" res="<<res << endl;
				
				
			}
			else
			{
				res=write(wfd, iter2->second, 1000); 
				cout <<"key="<<iter2->first << " 拼合第" << i << "个文件" << " res=" << res << endl;
			}
			++i;
			delete[] iter2->second;//释放图片数据
		}
		close(wfd);
		 
		cout << "校验通过写入日志和数据库，拼合完毕" << endl;
		iter->second->getWriteLog(); 
		//cout << "删map" << endl;
		delete iter->second;
		this->useridMap->erase(imageokt.user_id);
		//cout << "拼完了" << endl;
		imageback.flag = 0;//拼合成功
		memcpy(buf + sizeof(HEAD), &imageback, head.length);
		
		
	} 
	else
	{
		int index=1;//对比下标
		cout << "校验失败，客户端需要重发：" << endl;
		map<int, char*>::iterator iter2;//文件数据map迭代器
		string lose_nums;//丢失序列
		char lose_nums2[20] = {0};
		for (iter2 = iter->second->getImg_dataMap().begin(); iter2 != iter->second->getImg_dataMap().end(); iter2++)
		{
			while (iter2->first != index)
			{
				cout <<"用户"<< userid << ", 第" << index << "号文件包丢失" << endl;
				sprintf(lose_nums2,"%d#", index);
				lose_nums += lose_nums2;
				++index;
			}
		    ++index;
		}
		cout << lose_nums << endl;
		cout << "img_name"<< imageokt .img_name<<endl;
		imageback.flag = 1;//数据包丢失
		strcpy(imageback.img_path, iter->second->getImg_path().c_str());
		strcpy(imageback.img_nums, lose_nums.c_str());
		imageback.user_id = userid;
		memcpy(buf + sizeof(HEAD), &imageback, head.length);
		//cout << "校验错误，客户端需要重发fd=" << fd << endl;
		
	}
	IPC::getInstance()->sendToFront(buf);
}
