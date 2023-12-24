#pragma once
#include <iostream>
#include <list>
#include <queue>
#include <pthread.h>
#include <algorithm>
#include "CBaseTask.h"
#include "pthread.h"
#include "MyDataBase.h"
using namespace std;
#define MINNUM 8
#define MAXNUM 16
//线程池类
class CThreadPool
{
public:
	CThreadPool(const int min = MINNUM, const int max = MAXNUM);//默认参数
	~CThreadPool();
	//添加任务到任务队列
	void pushTask(CBaseTask* task);
	//移除任务
	CBaseTask* popTask();
	//线程从 忙碌列表移动到空闲列表
	void moveToIdle(pthread_t id);
	//线程从 空闲列表移动到忙碌列表
	void moveToBusy(pthread_t id);
	//验证任务队列是否为空
	bool queueIsEmpty();
	//互斥量 操作
	void lock();//上锁
	void unlock();//解锁
	//条件变量 操作
	void wait();//阻塞
	void wakeup();//唤醒
	static void* RunTime(void* arg);
private:
	int threadMinNum;//线程最小数量
	int threadMaxNum;//线程最大数量
	queue<CBaseTask*> taskQueue;//任务队列
	list<pthread_t> idleList;//空闲列表 pthread_t线程id号
	list<pthread_t> busyList;//忙碌列表
	pthread_mutex_t mutex;//线程互斥量 用于解决线程安全问题
	pthread_cond_t cond;//线程条件变量 产生线程条件变量（阻塞、唤醒）
};

