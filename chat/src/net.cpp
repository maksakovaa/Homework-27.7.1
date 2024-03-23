#include "net.h"

net::net()
{
	localeSetup();
	readConfig();
#if defined (_WIN32) || defined (_WIN64)
	convertIp();
	initWinsock();
#endif
	createSocket();
	connectToServer();
}

net::~net()
{
#if defined (_WIN32) || defined (_WIN64)
	closesocket(ClientSock);
	WSACleanup();
#elif defined (__linux__)
	close(socket_fd);
#endif
}

void net::readConfig()
{
	std::ifstream config(netCfgPath);
	if (config.is_open())
	{
		string str;
		while (std::getline(config, str))
		{
			if (strncmp("server_ip", str.data(), (int)strlen("server_ip")) == 0)
				server_ip = str.erase(0, str.find(delim) + delim.length());
			if (strncmp("chat_port", str.data(), (int)strlen("chat_port")) == 0)
				chat_port = str.erase(0, str.find(delim) + delim.length());
		}
		config.close();
	}
	else
	{
		server_ip = "127.0.0.1";
		chat_port = "9999";
		saveConfig();
	}
}

void net::saveConfig()
{
	std::ofstream config(netCfgPath, std::ios::trunc);
	if (!config.is_open())
	{
		cout << "[" << curDateTime() << "] ERROR: Ошибка открытия файла!" << endl;
	}
	else
	{
		config << "server_ip = ";
		config << server_ip;
		config << "\n";
		config << "chat_port = ";
		config << chat_port;
		config << "\n";
		config.close();
	}
}

#if defined (_WIN32) || defined (_WIN64)

inline void net::convertIp()
{
	inet_pton(AF_INET, server_ip.data(), &ip_to_num);
}

inline void net::initWinsock()
{
	if ((erStat = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		cout << "[" << curDateTime() << "] ERROR: " << erStat << "WSAStartup failed" << endl;
		exit(1);
	}
	else
	{
		cout << "[" << curDateTime() << "] WSAStartup: " << wsaData.szSystemStatus << endl;
	}
}
#endif

inline void net::createSocket()
{
#if defined (_WIN32) || defined (_WIN64)
	ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		cout << "[" << curDateTime() << "] ERROR: " << WSAGetLastError() << " Ошибка создания сокета." << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(1);
	}
#elif defined (__linux__)
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_fd == -1)
	{
		cout << "[" << curDateTime() << "] ERROR: Ошибка создания сокета." << endl;
		exit(1);
	}
#endif
}

void net::connectToServer()
{
#if defined (_WIN32) || defined (_WIN64)
	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(stoi(chat_port));

	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "[" << curDateTime() << "] ERROR: " << WSAGetLastError() << " Ошибка подключения к серверу." << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(1);
	}

#elif defined (__linux__)
	srvaddress.sin_addr.s_addr = inet_addr(server_ip.data());
	srvaddress.sin_port = htons(stoi(chat_port));
	srvaddress.sin_family = AF_INET;

	connection = connect(socket_fd, (struct sockaddr*)&srvaddress, sizeof(srvaddress));

	if (connection == -1)
	{
		cout << "[" << curDateTime() << "] ERROR: Ошибка подключения к серверу." << endl;
		exit(1);
	}
#endif
}

void net::sendRequest(const string& reqName)
{
	bzero(package, pkg_length);
	strcpy(package, reqName.data());
#if defined (_WIN32) || defined (_WIN64)
	int packet_size = send(ClientSock, package, sizeof(package), 0);
	if (packet_size == SOCKET_ERROR)
	{
		cout << "[" << curDateTime() << "] ERROR: " << WSAGetLastError() << " Ошибка отправки запроса на сервер." << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(1);
	}
#elif defined (__linux__)
	ssize_t bytes = write(socket_fd, package, sizeof(package));
#endif
}

void net::getRequest()
{
	bzero(package, pkg_length);
#if defined (_WIN32) || defined (_WIN64)
	int packet_size = recv(ClientSock, package, sizeof(package), 0);

	if (packet_size == SOCKET_ERROR)
	{
		cout << "[" << curDateTime() << "] ERROR: " << WSAGetLastError() << " Ошибка получения ответа от сервера." << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(1);
	}
#elif defined (__linux__)
	read(socket_fd, package, sizeof(package));
#endif
}

bool net::isExit()
{
	if (strncmp("q", package, sizeof("q") - 1) == 0)
		return true;
	else
		return false;
}

void net::getUsrBase(std::vector<string>* users)
{
	sendRequest("GET_USRBASE");
	users->clear();
	int i = 0;
	while (strncmp("USRBASE_END", package, sizeof("USRBASE_END") - 1) != 0)
	{
		bzero(package, pkg_length);
		getRequest();
		if (strncmp("USRBASE_END", package, sizeof("USRBASE_END") - 1) == 0)
		{
			break;
		}
		else
		{
			string temp = package;
			users->push_back(temp);
		}
	}
}
void net::getAllMsgBase(std::vector<string>* sMsg, const string& uuid)
{
	sendRequest("GET_ALL_MSGBASE<|>" + uuid);
	sMsg->clear();
	while (strncmp("MSGBASE_END", package, sizeof("MSGBASE_END") - 1) != 0)
	{
		bzero(package, pkg_length);
		getRequest();
		if (strncmp("MSGBASE_EMPTY", package, sizeof("MSGBASE_EMPTY") - 1) == 0)
		{
			cout << "[" << curDateTime() << "] MSGBASE_EMPTY" << endl;
			break;
		}
		else if (strncmp("MSGBASE_END", package, sizeof("MSGBASE_END") - 1) == 0)
		{
			cout << "[" << curDateTime() << "] База сообщений обновлена." << endl;
			break;
		}
		else
		{
			string temp = "";
			temp.append(package);
			sMsg->push_back(temp);
		}
	}
}

void net::getPerMsgBase(std::vector<string>* sMsg, const string& uuid)
{
	sendRequest("GET_PER_MSGBASE<|>"+uuid);
	sMsg->clear();
	while (strncmp("MSGBASE_END", package, sizeof("MSGBASE_END") - 1) != 0)
	{
		bzero(package, pkg_length);
		getRequest();
		if (strncmp("MSGBASE_EMPTY", package, sizeof("MSGBASE_EMPTY") - 1) == 0)
		{
			cout << "[" << curDateTime() << "] MSGBASE_EMPTY" << endl;
			break;
		}
		else if (strncmp("MSGBASE_END", package, sizeof("MSGBASE_END") - 1) == 0)
		{
			cout << "[" << curDateTime() << "] База сообщений обновлена." << endl;
			break;
		}
		else
		{
			string temp = "";
			temp.append(package);
			sMsg->push_back(temp);
		}
	}
}

void net::regUser(const string& usrPkg)
{
	string request = "REG_USER<|>";
	request += usrPkg;
	sendRequest(request);
}

void net::delUser(const string& uuid)
{
	string request = "DEL_USER<|>";
	request += uuid;
	sendRequest(request);
}

void net::setUsrPwd(const string& usrPkg)
{
	string request = "CHG_PWD<|>";
	request += usrPkg;
	sendRequest(request);
}

void net::setAllMsgStatus(const string& packet)
{
	string request = "SET_AMSG_STATUS<|>";
	request += packet;
	sendRequest(request);
}

void net::setPerMsgStatus(const string& packet)
{
	string request = "SET_PMSG_STATUS<|>";
	request += packet;
	sendRequest(request);
}
