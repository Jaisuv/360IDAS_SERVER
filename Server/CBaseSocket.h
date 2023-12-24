#pragma once
#include <sys/types.h>    
#include <sys/socket.h>
class CBaseSocket
{
public:
	CBaseSocket(unsigned short port);
	int getsocketfd();
	void startConnect();
	virtual void work()=0;//连接
	virtual void socketDisConnect() = 0;//断开连接
protected:
	int socketfd;

};

