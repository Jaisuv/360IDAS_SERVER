#pragma once
#include "CBaseTask.h"
#include "CImage.h"
#include "protocol.h"
#include <map>
class CImageSpliceTask :
    public CBaseTask //图片拼合任务
{
public:
    CImageSpliceTask(char* data, int length, map<int, CImage*>* useridMap);
    ~CImageSpliceTask();

    // 通过 CBaseTask 继承
    void working() override;
private:
    map<int, CImage*>* useridMap;
};

