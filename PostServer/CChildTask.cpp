#include "CChildTask.h"
#include "protocol.h"
#include "IPC.h"
#include <iostream>
#include <unistd.h>
using namespace std;
CChildTask::CChildTask(char* data, int length):CBaseTask(data,length)
{
}

CChildTask::~CChildTask()
{
}

void CChildTask::working()
{
	cout << "任务无法识别，请检查协议" << endl;
}
