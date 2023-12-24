#pragma once
#include "CBaseSocket.h"
#include "CHostAddress.h"

#include <arpa/inet.h>
#include <unistd.h>
class CTCPServer :public CBaseSocket
{
public:
	CTCPServer(unsigned short port);
	void work();//连接
	void socketDisConnect();//断开连接
private:
	CHostAddress* hostAddress;//地址
};

