#pragma once
//数据库单例
#include <stdio.h>
#include <sqlite3.h>
#include <iostream>
using namespace std;
class MyDataBase
{
public:
	static MyDataBase* GetInstance();
	/*
	* 函数名称：insertDelUpd 
	* 函数作用：对表的数据 增删改
	* 函数参数：sql 操作语句
	* 函数返回：0成功 其他 sql错误
	*/
	int insertDelUpd(string sql);
	/*
	* 函数名称：getData
	* 函数作用：对表的数据 查询
	* 函数参数：sql 操作语句
	*			row 行数
	*			col 列数
	* 函数返回：0成功 其他 sql错误
	*/
	int getData(string sql, char**& result, int& row, int& col);
private:
	MyDataBase();//构造私有化
	~MyDataBase();//析构
	static MyDataBase* instance;//实例
	sqlite3* db;//数据库
};


