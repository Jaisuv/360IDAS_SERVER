#include "MyDataBase.h"

MyDataBase* MyDataBase::instance = new MyDataBase(); 
MyDataBase* MyDataBase::GetInstance()
{
	return MyDataBase::instance;
}
/*
* 函数名称：insertDelUpd
* 函数作用：对表的数据 增删改
* 函数参数：sql 操作语句
* 函数返回：0成功 其他 sql错误
*/
int MyDataBase::insertDelUpd(string sql)
{
    char* errmsg;
    int res = sqlite3_exec(this->db, sql.c_str(), nullptr, nullptr, &errmsg);
    if (SQLITE_OK == res)
    {
        return 0;
    }
    cout << sqlite3_errmsg(this->db) << "--MyDataBase::insertDelUpd()--26"<<endl;
    cout << sqlite3_errcode(this->db) << "--MyDataBase::insertDelUpd()--27"<<endl;
    return res;
}
/*
* 函数名称：getData
* 函数作用：对表的数据 查询
* 函数参数：sql 操作语句
*			row 行数
*			col 列数
* 函数返回：0成功 其他 sql错误
*/
int MyDataBase::getData(string sql, char**& result, int& row, int& col)
{
    char* errmsg;
    int res = sqlite3_get_table(this->db, sql.c_str(), &result, &row, &col, &errmsg);
    if (SQLITE_OK == res)
    {
        return 0;
    }
    cout << sqlite3_errmsg(this->db) << "--MyDatabase::getData()--46"<<endl;
    cout << sqlite3_errcode(this->db) << "--MyDatabase::getData()--47"<<endl;
    return res;
}

MyDataBase::MyDataBase()
{
    int res = sqlite3_open("server.db", &this->db);//开数据库
    if (SQLITE_OK == res)
    {
        cout<< "-- MyDatabase::MyDatabase-- db OpenSucess--19 --"<<endl;
    }
    else {
        cout << sqlite3_errmsg(this->db) << "--MyDataBase::MyDataBase()--22"<< endl;
        cout << sqlite3_errcode(this->db) << "--MyDataBase::MyDataBase()--23"<< endl;
    }
}

MyDataBase::~MyDataBase()
{
    sqlite3_close(this->db);
}
