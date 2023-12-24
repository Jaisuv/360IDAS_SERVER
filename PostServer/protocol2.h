#pragma once
#define LOGIN		0 //登录
#define LOGRESPOND	1 //登录返回
#define REGISTER	2 //注册
#define REGRESPOND	3 //注册返回
typedef struct head_t//请求头
{
	int type;//业务类型
	int length;//业务长度
	int num;//流水号
}HEAD;
typedef struct login_t//登录请求包
{
	char id[8];//账号
	char pwd[32];//密码
}LOGT;
typedef struct backmsg_t //1、登录 2、注册 3、录制 4、播放记录上传 返回包 
{
	int flag;//0 成功  1 失败
	char name[20];//昵称
}BACKMSGT;
typedef struct reg_t  //注册请求包
{
	char id[8];//账号
	char name[20];//昵称
	char pwd[32];//密码
}REGT;


typedef struct video_t //1、录制请求包 2、查询返回包成员 3、播放记录上传请求包
{
	int framenum;//视频总帧数
	int currentframe;//视频当前帧/上次观看帧数
	int user_id;//用户id
	int equipment_id;//设备id
	char video_name[20];//视频名称
	char video_cover[50];//视频封面路径
	char video_path[50];//视频路径
}VIDEOT;
typedef struct serch_t//查询请求包
{
	int user_id;//用户id
	int equipment_id;//设备id
	int currentPage;//当前页
}SERCHT;
typedef struct serchrespond_t//查询返回包
{
	int num;//视频个数
	VIDEOT videoarr[4];//视频结构体数据
}SERCHREST;
