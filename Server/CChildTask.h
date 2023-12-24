#pragma once
#include "CBaseTask.h"
#include <iostream>
#include <unistd.h>
using namespace std;
//子任务类
class CChildTask :
    public CBaseTask
{
public:
    CChildTask(char * data);
    ~CChildTask();

    // 通过 CBaseTask 继承
    void working() override;


};

