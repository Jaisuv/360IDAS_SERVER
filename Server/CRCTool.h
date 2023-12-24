#pragma once

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <fcntl.h>  
#include <sys/stat.h>  
#include <iostream>
#include <unistd.h>
#include <string.h>
using namespace std;

//#define BUFSIZE		4096     //1024*4 


class CRCTool
{
public:
	void init_crc_table(void);	//初始化crc表,生成32位大小的crc表 
	/*
	函数功能: 计算buffer的crc校验码
	函数参数: buffer: 需要计算的数据
			  size: buffer的字节数
	函数返回: 生成的crc校验码
	*/
	unsigned int calc_buf_crc(unsigned char* buffer, unsigned int size);
	static CRCTool* getInstance();

private:
	unsigned int crc_table[256];		//crc生成多项式

	/*第一次传入的值需要固定,如果发送端使用该值计算crc校验码,
	**那么接收端也同样需要使用该值进行计算*/
	unsigned int crc;		//固定值
	CRCTool();
	static CRCTool* instance;
};

