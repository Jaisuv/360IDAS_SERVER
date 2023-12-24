#pragma once
#include "CBaseTask.h"
class CRegisterTask :
    public CBaseTask
{
public:
    CRegisterTask(char* data, int length);
    ~CRegisterTask();
    // 通过 CBaseTask 继承
    void working() override;
};

