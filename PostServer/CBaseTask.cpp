#include "CBaseTask.h"
CBaseTask::CBaseTask(char* data,int length)
{
	bzero(this->data, sizeof(this->data));
	memcpy(this->data, data,sizeof(this->data));
	this->length = length;
}
CBaseTask::~CBaseTask()
{
}