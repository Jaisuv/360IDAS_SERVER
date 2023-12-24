#include "CBaseSocket.h"
#include <iostream>
using namespace std;
CBaseSocket::CBaseSocket(unsigned short port)
{
	cout << "新建了一个socket" << endl;
}
int CBaseSocket::getsocketfd()
{
	return this->socketfd;
}
void CBaseSocket::startConnect()
{
	//打开网络
	this->socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		perror("socker error:");
		exit(0);
	}
	else
	{
		cout << "服务器通道 socketfd=" << socketfd << endl;
		this->work();
	}
	

}

