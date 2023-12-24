#pragma once
#include <netinet/in.h>
//主机地址类
class CHostAddress
{
public:
	CHostAddress(unsigned short port);

	struct sockaddr_in getAddr_in();
	struct sockaddr* getAddr();
	int getlen();
	void setlen(int value);
	unsigned short getport();
	void setport(unsigned short value);
private:
	struct sockaddr_in s_addr;
	int len;
	unsigned short port;//端口是一个短整形
};

