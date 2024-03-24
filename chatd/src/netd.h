#pragma once
#include "sql_db.h"

using std::cout;
using std::endl;

extern Logger* Log;
extern Settings* Config;
extern UsrBase* UserBase;
extern MsgBase* AllMsgBase;
extern SQL_DB* db_connect;

class net
{
public:
	net();
	~net();
	void getConfig();
#if defined (_WIN32) || defined (_WIN64)
	void convertIp();
	void initWinsock();
	void disconnect();
#endif
	void createSocket();
	void bindSocket();
	void servlisten();
	void acceptConnection();
	void transmission();
	void sendRequest(const string& reqName);
	void getRequest();
	bool isExit();
	void requestAPI();
	void cutRequestHeader(string& request);
	void sendUsrBase();
	void sendPerMsgBase();
	void sendAllMsgBase();
	void regUser();
	void regMSG();
	void delUsr();
	void chgPwd();
	void setPMStatus();
	void setAMStatus();
private:
	void logging(const string& entry);
	string server_ip, chat_port, reqName, delim{"<|>"};
	static const int pkg_length = 1024;
	char package[pkg_length];
	int erStat;
#if defined (_WIN32) || defined (_WIN64)
	in_addr ip_to_num;
	WSADATA wsaData;
	SOCKET ServSock;
	SOCKET ClientConn;
#elif defined (__linux__)
	struct sockaddr_in srvaddress, client;
	socklen_t length;
	int socket_fd, connection;
#endif
};