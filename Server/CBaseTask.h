#pragma once
#include <string.h>
//任务基类
#define DATASIZE 1500
class CBaseTask
{
public:
	CBaseTask(char * data);
	virtual ~CBaseTask();
	virtual void working() = 0;

protected:
	char data[DATASIZE];
	
};

