#include "CChildTask.h"

CChildTask::CChildTask(char* data):CBaseTask(data)
{
}

CChildTask::~CChildTask()
{
}

void CChildTask::working()
{
	cout << this->data << "  <<<<正在执行......" << endl;
	sleep(15);
}
