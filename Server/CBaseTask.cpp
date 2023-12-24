#include "CBaseTask.h"
CBaseTask::CBaseTask(char* data)
{
	bzero(this->data, sizeof(this->data));
	memcpy(this->data, data,sizeof(this->data));
}
CBaseTask::~CBaseTask()
{
}