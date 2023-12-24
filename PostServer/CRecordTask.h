#pragma once
#include "CBaseTask.h"
class CRecordTask :
    public CBaseTask
{
public:

    CRecordTask(char* data, int length);
    ~CRecordTask();
    // 通过 CBaseTask 继承
    void working() override;
};

