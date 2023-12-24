#pragma once
#include "CBaseTask.h"
class CSearchImageTask :
    public CBaseTask
{
public:
    CSearchImageTask(char* data, int length);
    ~CSearchImageTask();
    // 通过 CBaseTask 继承
    void working() override;
};

