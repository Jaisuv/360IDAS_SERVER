#pragma once
#include "CBaseTask.h"
class CLoginTask :
    public CBaseTask
{
public:
    CLoginTask(char* data, int length);
    ~CLoginTask();

    // 通过 CBaseTask 继承
    void working() override;
};

