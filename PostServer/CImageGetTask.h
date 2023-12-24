#pragma once
#include "CBaseTask.h"
#include "CImage.h"
#include "protocol.h"
#include <map>
using namespace std;
class CImageGetTask:public CBaseTask
{
public:
    CImageGetTask(char* data, int length, map<int, CImage*>* useridMap);
    ~CImageGetTask();

    // 通过 CBaseTask 继承
    void working() override;
private:
    map<int, CImage*> *useridMap;
};

