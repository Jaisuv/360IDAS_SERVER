#pragma once
#define LOGIN		0 //登录
#define LOGRESPOND	1 //登录返回
#define REGISTER	2 //注册
#define REGRESPOND	3 //注册返回
#define VIDEO	4 //视频信息上传
#define VIDEORESPOND	5 //视频信息上传返回
#define RECORD	6 //播放记录上传
#define RECORDRESPOND	7 //播放记录上传返回
#define SEARCH	8 //查询请求
#define SEARCHRESPOND	9 //查询结果返回
#define IMAGE	10 //图片信息上传
#define IMAGECOMPLETE	11 //图片全部上传完成
#define IMAGERESPOND 12  //图片重发请求
#define SERCHIMAGE 13 //查询特征图片
#define SERCHIMAGERESPOND 14 //特征图片查询返回
#define OFFLINE 15 //客户端下线请求
#define HEARTBEAT 16 //心跳包
typedef struct head_t//请求头
{
    int type;//业务类型
    int length;//业务长度a
    unsigned int CRCnum;//CRC校验码
}HEAD;
typedef struct login_t//登录请求包
{
    char id[10];//账号
    char pwd[40];//密码
}LOGT;
typedef struct backmsg_t //1、登录 2、注册 3、录制 4、播放记录上传 返回包
{
    int flag;//0 成功  1 失败
    char name[30];//昵称
    int user_id;//用户id
}BACKMSGT;
typedef struct reg_t  //注册请求包
{
    char id[10];//账号
    char name[30];//昵称
    char pwd[40];//密码
}REGT;


typedef struct video_t //1、录制请求包 2、查询返回包成员 3、播放记录上传请求包
{
    int framenum;//视频总帧数
    int currentframe;//视频当前帧/上次观看帧数
    int equipment_id;//设备id
    int user_id;//用户id
    char video_name[40];//视频名称
    char video_cover[50];//视频封面路径
    char video_path[50];//视频路径
}VIDEOT;
typedef struct serch_t//查询请求包 视频查询请求 图片查询请求
{
    int equipment_id;//设备id
    int currentPage;//当前页
    int user_id;//用户id
}SEARCHT;
typedef struct serchrespond_t//查询返回包
{
    int num;//视频个数
    VIDEOT videoarr[4];//视频结构体数据
}SEARCHREST;

typedef struct image_t//图片信息上传
{
    char data[1000];//图片数据
    int img_num;//单个图片序号
    int img_length;//单个图片长度
    int img_allBytes;//文件总字节数
    int img_allNums;//零碎文件个数
    char img_name[40];//图片名称
    int img_type;//图片类型
    int user_id;//用户id
    int equipment_id;//设备id
    char img_path[50];//图片路径
}IMAGET;

typedef struct imageOK_t//图片信息上传完成
{
    char img_name[40];//图片名称
    int img_type;//图片类型
    int user_id;//用户id
}IMAGEOKT;

typedef struct imageback_t//图片拼合完成返回包
{
    int flag;//0 拼合完成  1 数据包丢失
    char img_path[50];//图片路径
    char img_nums[100];//所有丢失包的序号
    int user_id;//用户id
}IMAGEBACKT;

typedef struct imageserch_t //图片查询返回的信息包  作为图片查询返回的属性
{
    int image_type;//图片类型
    char image_name[40];//图片名称
    char image_path[50];//图片路径
}IMAGESAERCHT;

typedef struct imageserchRespond_t //图片查询结果返回
{
    int image_num;
    IMAGESAERCHT image_arr[4];//图片信息数组
}IMAGESEARCHREST;
