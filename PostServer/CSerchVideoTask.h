#pragma once
#include "CBaseTask.h"
class CSerchVideoTask :
    public CBaseTask //视频查找任务
{
public:
    CSerchVideoTask(char* data, int length);
    ~CSerchVideoTask();
    // 通过 CBaseTask 继承
    void working() override;
};

