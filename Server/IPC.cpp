#include "IPC.h"
#include "protocol.h"
IPC* IPC::instance = nullptr;
IPC* IPC::getInstance()
{
    if (IPC::instance == nullptr)
    {
        IPC::instance = new IPC();
    }
    return IPC::instance;
}

//信号量创建
int IPC::sem_create(key_t key, int num_sems)
{
	umask(0);
	int id = semget(key, num_sems, IPC_CREAT | 0777);
	if (id < 0)
	{
		perror("semget error");
	}
	else
	{
		cout << "信号量创建成功" << endl;
	}
	return id;
}
//信号量初始化 sem_num 信号量下标  value 值
void IPC::sem_setval(int sem_id, int sem_num, int value)
{
	union semun arg;
	arg.val = value;
	int res = semctl(sem_id, sem_num, SETVAL, arg);
	if (res < 0)
	{
		perror("semctl error");
	}
}
//信号量P操作 -1
void IPC::sem_p(int sem_id, int sem_num)
{
	struct sembuf buf = { sem_num,-1,SEM_UNDO };
	int res = semop(sem_id, &buf, 1);
	if (res < 0)
	{
		perror("sem_p error");
	}
}
//信号量V操作 +1
void IPC::sem_v(int sem_id, int sem_num)
{
	struct sembuf buf = { sem_num,+1,SEM_UNDO };
	int res = semop(sem_id, &buf, 1);
	if (res < 0)
	{
		perror("sem_v error");
	}
}

void IPC::sendToPost(char * data)
{
	MSGBUF msgbuf = { 0 };
	void* shmaddr = NULL;
	int i = 0;
	while (1)
	{
		shmaddr = shmat(shmid, NULL, 0);
		memcpy(arr, shmaddr, sizeof(arr));
		for (int i = 0; i < BLOCK_NUM; i++)
		{
			if (arr[i] == 0)
			{
				break;
			}
		}
		if (i == BLOCK_NUM)
		{
			shmdt(shmaddr);//断开连接
			continue;//共享内存均被占用 继续等待 直到有空闲位置出现
		}

		//加锁 p操作
		sem_p(semid, i);
		//操作数据区 跨过索引区操作数据区，利用下标来计算
		memcpy((void*)(shmaddr + sizeof(arr) + DATA_SIZE * i), data, DATA_SIZE);
		//进程内本地数组操作
		arr[i] = 1;
		//操作索引区 只能拷贝数组中被修改的哪一个元素
		memcpy((void*)(shmaddr + i * sizeof(int)), &arr[i], sizeof(int));
		shmdt(shmaddr);
		sem_v(semid, i);

		msgbuf.mtype = 1;
		sprintf(msgbuf.mtext, "%d", i);
		if (msgsnd(msgid, &msgbuf, sizeof(msgbuf.mtext), 0) == -1)
		{
			perror("msgsnd error");
		}
		cout << "消息队列发送 i=" << i << endl;
		bzero(&msgbuf, sizeof(MSGBUF));
		bzero(&arr[i], sizeof(int));
		break;
	}
}

void IPC::getFromPost(char* data)
{
	MSGBUF msgbuf = { 0 };
	void* shmaddr = NULL;
	int i = -1;
	if (msgrcv(msgid, &msgbuf, sizeof(msgbuf.mtext), 2, 0) == -1)//接收
	{
		perror("msgrcv error");
		return;
	}
	i = atoi(msgbuf.mtext);
	if (i == -1)
	{
		return;
	}

	shmaddr = shmat(shmid, NULL, 0);

	memcpy(&arr[i], (void*)(shmaddr + i * sizeof(int)), sizeof(int));
	if (arr[i] == 1)
	{
		//加锁 p操作
		sem_p(semid, i);
		//操作数据区 跨过索引区操作数据区，利用下标来计算
		memcpy(data, (void*)(shmaddr + sizeof(arr) + DATA_SIZE * i), DATA_SIZE);
		//清空数据
		memset((void*)(shmaddr + sizeof(arr) + DATA_SIZE * i), 0, DATA_SIZE);
		//清空索引
		memset((void*)(shmaddr + i * sizeof(int)), 0, sizeof(int));
		shmdt(shmaddr);
		sem_v(semid, i);
	}

	bzero(&msgbuf, sizeof(MSGBUF));
	bzero(&arr[i], sizeof(int));
}
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
void IPC::ctlCount(int index, int mode)
{
	void* shmaddr2 = NULL;

	//加锁 p操作
	sem_p(semid2, index);
	shmaddr2 = shmat(shmid2, NULL, 0);
	memcpy(workNum, shmaddr2, sizeof(workNum)); //拷贝数据
	//进程内本地数组操作 
	if (mode == 0)
		++workNum[index];
	else if (mode == 1)
		--workNum[index];
	//操作索引区 只能拷贝数组中被修改的哪一个元素
	memcpy((void*)(shmaddr2 + index * sizeof(int)), &workNum[index], sizeof(int));
	shmdt(shmaddr2);
	sem_v(semid2, index);
	bzero(&workNum[index], sizeof(int));
}

void IPC::printfLog(int allnum)
{
	cout << endl << "######################## 实时日志 ########################" << endl;
	cout << "    有效连接数：" << allnum << endl;
	bzero(workNum, sizeof(workNum));//先清零 
	void* shmaddr2 = shmat(shmid2, NULL, 0);
	memcpy(workNum, shmaddr2, sizeof(workNum)); //全部读出
	cout << "    接收数据包：" << workNum[0] << "  发送数据包  ：" << workNum[1] << endl;
	cout << "    用户登入数：" << workNum[2] << endl;
	cout << "    请求登录：" << workNum[3] << "  请求注册：" << workNum[4] << "  请求下线：" << workNum[5] << endl;
	cout << "    上传视频数据：" << workNum[6] << "  上传视频记录：" << workNum[7] << "  查询视频数据：" << workNum[8] << endl;
	cout << "    上传零碎文件：" << workNum[9] <<"    上传特征图片：" << workNum[10] << "  查询图片数据：" << workNum[11] << endl;
	cout << "##########################################################" << endl;
}

IPC::IPC()
{
	bzero(arr,sizeof(arr));
	umask(0);
	//创建共享内存
	this->shmid = shmget((key_t)1001, sizeof(arr) + DATA_SIZE * BLOCK_NUM, IPC_CREAT | 0777);
	if (shmid < 0)
	{
		perror("shmget error");
		exit(0) ;
	}
	cout << "共享内存创建成功 "<<shmid << endl;
	//创建消息队列
	this->msgid = msgget((key_t)1002, IPC_CREAT | 0777);
	if (msgid < 0)
	{
		perror("msgget error");
		exit(0);
	}
	cout << "消息队列创建成功 " <<msgid<< endl;
	//创建信号量
	this->semid = sem_create((key_t)1003, BLOCK_NUM);
	//初始化
	for (int i = 0; i < BLOCK_NUM; i++)
	{
		sem_setval(semid, i, 1);
	}
	cout << "信号量初始化完毕 " <<semid<< endl;

	//创建日志共享内存
	this->shmid2 = shmget((key_t)2001, sizeof(workNum) , IPC_CREAT | 0777);
	if (shmid < 0)
	{
		perror("log shmget error");
		exit(0);
	}
	cout << "日志专用共享内存创建成功 " << shmid2 << endl;
	bzero(workNum, sizeof(workNum));//先清零 
	void* shmaddr2 = shmat(shmid2, NULL, 0); 
	memcpy(shmaddr2, workNum, sizeof(workNum)); //全部置零 
	//创建日志信号量
	this->semid2 = sem_create((key_t)2002, BLOCK_NUM);
	//初始化
	for (int i = 0; i < LOG_NUM; i++)
	{
		sem_setval(semid2, i, 1);
	}
	cout << "日志信号量初始化完毕 " << semid2 << endl;
}

IPC::~IPC()
{
}
