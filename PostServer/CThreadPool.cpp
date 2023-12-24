#include "CThreadPool.h"

CThreadPool::CThreadPool(const int min,const int max)
{
	this->threadMinNum = min;
	this->threadMaxNum = max;

	//互斥量初始化
	pthread_mutex_init(&this->mutex,NULL);
	//条件变量初始化
	pthread_cond_init(&this->cond, NULL);

	for (int i = 0; i < this->threadMinNum; i++)
	{
		pthread_t id;
		pthread_create(&id,NULL,RunTime,this);
		//添加到空闲列表
		this->idleList.push_back(id);
		cout<<"添加到空闲列表"<<i<< endl;
	}
}

CThreadPool::~CThreadPool()
{
}

void CThreadPool::pushTask(CBaseTask* task)
{
	this->lock();
	this->taskQueue.push(task);
	//cout << "添加了一个任务" << endl;
	//极端情况  空闲列表为空，任务队列不为空
	if (this->idleList.empty() && (this->idleList.size() + this->busyList.size()) < this->threadMaxNum)
	{
		pthread_t id;
		pthread_create(&id, NULL, RunTime, this);
		//添加到空闲列表
		this->idleList.push_back(id);
		cout << "添加一个线程" << this->idleList.size() + this->busyList.size() << endl;
	}
	if (!this->idleList.empty())
	{
		this->wakeup();//每入队一个任务就唤醒一个线程
	}
	this->unlock();
	
}

CBaseTask* CThreadPool::popTask()
{
	CBaseTask *task=this->taskQueue.front();//获取队列头部
	this->taskQueue.pop();//删除队列头部
	return task;
}

void CThreadPool::moveToIdle(pthread_t id)
{
	list<pthread_t>::iterator iter;
	iter = find(this->busyList.begin(), this->busyList.end(), id);
	if (iter != this->busyList.end())
	{
		this->busyList.erase(iter);
		this->idleList.push_back(*iter);
		cout << "去空闲" << this->idleList.size() << endl;
	}
	//删线程
	//极端情况 任务队列为空，空闲列表空闲线程数量大于最小线程数
	if (this->taskQueue.empty() && this->idleList.size() > this->threadMinNum)
	{
		list<pthread_t>::iterator iter;
		iter = find(this->idleList.begin(), this->idleList.end(), id);//移除
		if (iter != this->idleList.end())
		{
			cout << "删除一个线程" << endl;
			this->idleList.erase(iter);
		}
	}
}

void CThreadPool::moveToBusy(pthread_t id)
{
	list<pthread_t>::iterator iter;
	iter = find(this->idleList.begin(), this->idleList.end(), id);
	if (iter != this->idleList.end())
	{
		this->idleList.erase(iter);
		this->busyList.push_back(*iter);
		cout << "去忙碌" <<this->idleList.size()<< endl;
	}
}

bool CThreadPool::queueIsEmpty()
{
	return this->taskQueue.empty();
}

void CThreadPool::lock()
{
	pthread_mutex_lock(&this->mutex);
}

void CThreadPool::unlock()
{
	pthread_mutex_unlock(&this->mutex);
}

void CThreadPool::wait()
{
	pthread_cond_wait(&this->cond,&this->mutex);
}

void CThreadPool::wakeup()
{
	pthread_cond_signal(&this->cond);
}

void* CThreadPool::RunTime(void* arg)
{
	pthread_t id = pthread_self();//获取线程自己的id
	bool endThread = false;
	//确保主线程与当前执行线程逻辑完全分离，当前执行线程执行结束后，id会自动释放
    //分离目的:为了声明这个线程不会阻塞主线程的逻辑,pthread_detach不会终止当前执行
	pthread_detach(id);

	CThreadPool* pool= (CThreadPool*)(arg);
	CBaseTask* t;
	while (1)
	{
		pool->lock();
		while (pool->queueIsEmpty())
		{
			pool->wait();//条件变量阻塞线程
		}
		pool->moveToBusy(id);//将线程自身 移动到忙碌
		t=pool->popTask();
		pool->unlock();//对敏感资源的操作已结束 解锁

		t->working();
		delete t;

		pool->lock();
		pool->moveToIdle(id);//将线程自身 移动到空闲
		pool->unlock();

		cout << "剩余任务数 " << pool->taskQueue.size() << " | 空闲线程数 " << pool->idleList.size() << " | 忙碌线程数" << pool->busyList.size() << endl;
		cout << "└-------------------------------------------------------┘" << endl;
	}
	return nullptr;
}
