#pragma once
#include <string>
#include <map>
#include <iostream>
#include "protocol.h"
using namespace std;
//图片类
class CImage
{
public:
	CImage(int img_allBytes, int img_allNums,int user_id, int img_type, int equipment_id, string img_name, string img_path);
	~CImage();
	map<int, char*>& getImg_dataMap();
	/*
	* 函数作用：比对文件总字节和当前字节,并且比对传入num和map的size
	* 返回值：相等 true 不相等 false
	*/
	bool compareBytesAndNum();
	/*
	* 函数名称：addBytes
	* 函数作用：对文件当前总字节数进行累加
	* 函数参数：int bytes 字节
	* 函数返回：
	*/
	void addBytes(int bytes);
	//获取总文件数量
	int getAllnums();
	//获取总文件字节
	int getAllBytes();
	//获取文件名
	string getImg_name();
	//获取文件路径
	string getImg_path();
	//写入日志
	void getWriteLog();
	//获取设备id
	string getEquipment_id();
private:
	int img_allBytes;//文件总字节数
	int currentBytes;//文件当前总字节数
	int user_id;//用户id
	int img_allNums;//零碎文件总个数
	int img_type;//图片类型
	int equipment_id;//设备id
	string img_name;//图片名称
	string img_path;//图片路径
	map<int, char* > img_dataMap;//图片信息map

};

