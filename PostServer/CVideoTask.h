#pragma once
#include "CBaseTask.h"
class CVideoTask :
    public CBaseTask
{
public:

    CVideoTask(char* data, int length);
    ~CVideoTask();
    // 通过 CBaseTask 继承
    void working() override;
};

