#include "CImageGetTask.h"
#include "MyLog.h"
#include "IPC.h"
CImageGetTask::CImageGetTask(char* data, int length, map<int, CImage*>* useridMap):CBaseTask(data,length)
{
	this->useridMap = useridMap;
}

CImageGetTask::~CImageGetTask()
{
}

void CImageGetTask::working()
{
	IPC::getInstance()->ctlCount(9, 0);//+零碎文件
	IMAGET imaget = { 0 };//图片结构体
	memcpy(&imaget,this->data+sizeof(HEAD),this->length);
	cout <<"imaget.img_num= " << imaget.img_num << " imaget.user_id=" << imaget.user_id << endl;

	map<int, CImage*>::iterator iter;
	iter=useridMap->find(imaget.user_id);
	if (iter == useridMap->end())
	{
		//cout << "没有这个key" << endl;
		this->useridMap->insert(make_pair(imaget.user_id,
	           new CImage(imaget.img_allBytes, imaget.img_allNums, imaget.user_id, imaget.img_type, imaget.equipment_id, imaget.img_name, imaget.img_path)));
		iter = useridMap->find(imaget.user_id); 
		char tmpchar[20] = { 0 };
		string info = "发送特征图片结构体数据包：\n图片名：";
		info += imaget.img_name;
		info += "\n图片大小：";
		sprintf(tmpchar,"%d", imaget.img_allBytes);
		info += tmpchar;
		info += "\n图片类型：";
		switch (imaget.img_type)
		{
		case 0:
			info += "行车模式特征图片";
			break;
		case 1:
			info += "红绿灯检测模式特征图片";
			break;
		case 2:
			info += "手动上传特征图片";
			break;
		}
		info += "\n图片路径：";
		info += imaget.img_path;
		info += "\n设备id：";
		sprintf(tmpchar, "%d", imaget.equipment_id);
		info += tmpchar;
		MyLog::GetInstance()->writeLogToDb("上传特征图片", 0, imaget.user_id, info); //上传特征图
	}
	
	int i=iter->second->getAllnums();
	cout << endl<<"┌-----------------------上传零碎文件--------------------┐" << endl;
	cout << "图片名：" << imaget.img_name << "--第" << imaget.img_num << "号包--总数" << imaget.img_num << endl;
	//cout << "allnums= " << i << endl;
	char* imgdata = new char[1000];
	memcpy(imgdata, imaget.data, 1000);
	iter->second->getImg_dataMap().insert(make_pair(imaget.img_num, imgdata));
	iter->second->addBytes(imaget.img_length);
	//cout << "size= " << iter->second->getImg_dataMap().size() << endl;
}

