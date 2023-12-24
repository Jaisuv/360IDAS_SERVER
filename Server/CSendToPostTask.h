#pragma once
#include "CBaseTask.h"
#include "protocol.h"
#include <map>
#include <iostream>
using namespace std;
class CSendToPostTask :
    public CBaseTask
{
public:
    CSendToPostTask(char* data,int fd, map<int, int>* cilentMap );//传入数据和 文本描述符
    ~CSendToPostTask();

    // 通过 CBaseTask 继承
    void working() override;
private:
    int fd;
    map<int, int> * cilentMap; //客户端在线map  fd，业务数
    map<int, int>::iterator iter;//迭代器
};

