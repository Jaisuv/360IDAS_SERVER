#pragma once
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
using namespace std;

#define BLOCK_NUM 200 //共享内存数据块 /信号量 数量
#define DATA_SIZE 1500 //数据大小
#define LOG_NUM 12 //日志数量
//联合体
union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds* buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short* array;  /* Array for GETALL, SETALL */
	struct seminfo* __buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};
typedef struct messagebuf
{
	long mtype;
	char mtext[20];
}MSGBUF;

class IPC
{
public:
	~IPC();
	static IPC* getInstance();
	//信号量创建 
	int sem_create(key_t key, int num_sems);
	//信号量P操作 -1
	void sem_p(int sem_id, int sem_num);
	//信号量V操作 +1
	void sem_v(int sem_id, int sem_num);
	void getFromFront(char * data);//从前置接收数据
	void sendToFront(char * data);//向前置发送数据
	/*
	* 函数名称：ctlCount
	* 函数作用：对指定区块进行++ 或 --
	* 函数参数：参数一：int index  操作下标
						0接受包数  1发送包数
						2用户登入数
						3请求登录 4 请求注册 5请求下线
						6上传视频数据  7上传视频记录  8查询视频数据
						9上传零碎文件  10上传特征图片  11查询图片数据
	*			参数二：int mode 操作模式 0 ++ ，1 --
	* 函数返回：
	*/
	void ctlCount(int index,int mode);
private:
	static IPC* instance;
	IPC();
	int arr[BLOCK_NUM];
	int shmid;//共享内存id
	int msgid;//消息队列id
	int semid;//信号量id
	//业务数组 
	int workNum[12];//int index  操作下标
						//0接受包数  1发送包数
						//2用户登入数
						//3请求登录 4 请求注册 5请求下线
						//6上传视频数据  7上传视频记录  8查询视频数据
						//9上传零碎文件  10上传特征图片  11查询图片数据
	int shmid2;//共享内存id2 
	int semid2;//信号量id2
};

