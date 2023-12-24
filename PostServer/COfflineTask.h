#pragma once
#include "CBaseTask.h"
class COfflineTask :
    public CBaseTask
{
public:
    COfflineTask(char* data, int length);
    ~COfflineTask();
    // 通过 CBaseTask 继承
    void working() override;
};

