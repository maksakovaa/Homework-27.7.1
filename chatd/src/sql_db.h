#pragma once
#include "MsgBased.h"

extern Settings* Config;
extern UsrBase* UserBase;
extern MsgBase* AllMsgBase;
extern MsgBase* PerMsgBase;
class SQL_DB
{
public:
	SQL_DB();
	~SQL_DB();
	void getConfig();
	void firstRun();
	void sendRequest(const string& request);
	void getRequest(const string& request);
	void getUserBase();
	void getPerMsgBase(const string& uuid);
	void getAllMsgBase(const string& uuid);
	void delUser(const string& uuid);
	void regUser(User& newUser);
	void chgPwd(const string& uuid, const string& pwd);
	void regPrivateMsg(Message& newMsg);
	void regAllMsg(Message& newMsg);
	void updPMStatus(int msgId, int status);
	void updAMStatus(const string& msgId, const string& forUser, const string& status);
private:
	MYSQL mysql;
	MYSQL_RES* result;
	MYSQL_ROW row;
	string m_query;
	string mysql_srv_ip;
	string mysql_login;
	string mysql_pass;
	string mysql_database;
	string mysql_table_users;
	string mysql_table_AM;
	string mysql_table_AMS;
	string mysql_table_PM;
};