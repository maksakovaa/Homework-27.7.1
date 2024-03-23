#pragma once
#include "sha256.h"

using std::getline;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;

extern string curDateTime();
extern void localeSetup();

class net
{
public:
	net();
	~net();
#if defined (_WIN32) || defined (_WIN64)
	void convertIp();
	void initWinsock();
#endif
	void createSocket();
	void connectToServer();
	void sendRequest(const string& reqName);
	void getRequest();
	bool isExit();
	void getUsrBase(std::vector<string>* users);
	void getAllMsgBase(std::vector<string>* sMsg, const string& uuid);
	void getPerMsgBase(std::vector<string>* sMsg, const string& uuid);
	void regUser(const string& usrPkg);
	void delUser(const string& uuid);
	void setUsrPwd(const string& usrPkg);
	void setAllMsgStatus(const string& packet);
	void setPerMsgStatus(const string& packet);
private:
	void readConfig();
	void saveConfig();
	string server_ip, chat_port, delim{" = "};
	static const int pkg_length = 1024;
	char package[pkg_length];
	int erStat;
#if defined (_WIN32) || defined (_WIN64)
	in_addr ip_to_num;
	WSADATA wsaData;
	SOCKET ClientSock;
	string netCfgPath = "settings.ini";
#elif defined (__linux__)
	int socket_fd, connection;
	struct sockaddr_in srvaddress, client;
	string netCfgPath = "/var/lib/chat/settings.ini";
#endif
};