#pragma once
#include "CBaseTask.h"
//子任务类
class CChildTask :
    public CBaseTask
{
public:
    CChildTask(char * data,int length);
    ~CChildTask();

    // 通过 CBaseTask 继承
    void working() override;


};

