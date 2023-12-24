#pragma once
#include "CBaseTask.h"
#include <map>
using namespace std;
class CSendToClient :
    public CBaseTask
{
public:
    CSendToClient(char* data,map<int,int> *cilentMap, map<int, int>* heartBeatMap);
    ~CSendToClient();

    // 通过 CBaseTask 继承
    void working() override;
private:
    map<int, int>* cilentMap;
    map<int, int>* heartBeatMap;
};

