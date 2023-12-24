#include "CHostAddress.h"

CHostAddress::CHostAddress(unsigned short port)
{ 
	this->len = sizeof(sockaddr_in);
	this->port = port;
	//协议族
	this->s_addr.sin_family = AF_INET;
	//端口号 0~65535 10000以下 系统默认  自定义应在10000以后
	this->s_addr.sin_port = htons(port);//字节转换函数
	//IP地址 INADDR_ANY 自动获取系统可用IP地址（服务器自己的IP地址）
	this->s_addr.sin_addr.s_addr = INADDR_ANY;
}
struct sockaddr_in CHostAddress::getAddr_in()
{
	return this->s_addr;
}
struct sockaddr* CHostAddress::getAddr()
{
	return (struct sockaddr*)&(this->s_addr);
}

int CHostAddress::getlen()
{
	return this->len;
}
void CHostAddress::setlen(int value)
{
	this->len = value;
}
unsigned short CHostAddress::getport()
{
	return this->port;
}
void CHostAddress::setport(unsigned short value)
{
	this->port = value;
}