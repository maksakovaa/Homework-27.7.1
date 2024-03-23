#include "netd.h"

net::net()
{
	getConfig();
#if defined (_WIN32) || defined (_WIN64)
	convertIp();
	initWinsock();
#endif
	createSocket();
	bindSocket();
}

net::~net()
{
#if defined (_WIN32) || defined (_WIN64)
	disconnect();
#elif defined (__linux__)
	close(socket_fd);
#endif
}

void net::getConfig()
{
	server_ip = Config->getServerIP();
	chat_port = Config->getChatPort();
}

#if defined (_WIN32) || defined (_WIN64)

void net::convertIp()
{
	erStat = inet_pton(AF_INET, server_ip.data(), &ip_to_num);

	if (erStat <= 0) {
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: Ошибка преобразования IP адреса в специальный цифровой формат." << endl;
		exit(1);
	}
}

void net::initWinsock()
{
	if ((erStat = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: Ошибка инициализации WSAStartup" << erStat;
		exit(1);
	}

	else
	{
		cout << "[" << curDateTime() << "] " << "[NET] WSAStartup: " << wsaData.szSystemStatus << endl;
	}
}

void net::disconnect()
{
	shutdown(ClientConn, SD_BOTH);
	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();
}

#endif

void net::createSocket()
{
#if defined (_WIN32) || defined (_WIN64)
	ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: " << WSAGetLastError() << " Ошибка создания сокета." << endl;
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else
		cout << "[" << curDateTime() << "] " << "[NET] Инициализация сокета успешна." << endl;
#elif defined (__linux__)
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: Ошибка создания сокета." << endl;
		exit(1);
	}
#endif
}

void net::bindSocket()
{
#if defined (_WIN32) || defined (_WIN64)
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(stoi(chat_port));

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
	if (erStat != 0) {
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: " << WSAGetLastError() << " Ошибка привязки сокета # " << endl;
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else
		cout << "[" << curDateTime() << "] " << "[NET] Привязка сокета выполнена." << endl;
#elif defined (__linux__)
	srvaddress.sin_addr.s_addr = htonl(INADDR_ANY);
	srvaddress.sin_port = htons(std::stoi(chat_port));
	srvaddress.sin_family = AF_INET;

	erStat = bind(socket_fd, (struct sockaddr*)&srvaddress, sizeof(srvaddress));
	if (erStat == -1)
	{
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: Ошибка привязки сокета." << endl;
		exit(1);
	}
	else
		cout << "[" << curDateTime() << "] " << "[NET] Привязка сокета выполнена." << endl;
#endif
}

void net::servlisten()
{
#if defined (_WIN32) || defined (_WIN64)
	erStat = listen(ServSock, SOMAXCONN);

	if (erStat != 0) {
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: Ошибка при постановке на приём данных # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else {
		cout << "[" << curDateTime() << "] " << "[NET] Ожидание подключений..." << endl;
	}
#elif defined (__linux__)
	erStat = listen(socket_fd, 20);
	if (erStat == -1)
	{
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: Ошибка при постановке на приём данных." << endl;
		exit(1);
	}
	else
	{
		cout << "[" << curDateTime() << "] " << "[NET] Ожидание подключений..." << endl;
	}
#endif
}

void net::acceptConnection()
{
#if defined (_WIN32) || defined (_WIN64)
	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));
	int clientInfo_size = sizeof(clientInfo);
	ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) {
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: " << WSAGetLastError() << " Сервер несмог принять данные от клиента." << endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		exit(1);
	}
	else
		cout << "[" << curDateTime() << "] " << "[NET] Соеединение успешно установлено" << endl;
#elif defined (__linux__)
	length = sizeof(client);
	connection = accept(socket_fd, (struct sockaddr*)&client, &length);
	if (connection == -1)
	{
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: Сервер несмог принять данные от клиента." << endl;
		exit(1);
	}
	else
		cout << "[" << curDateTime() << "] " << "[NET] Соеединение успешно установлено" << endl;
#endif
}

void net::transmission()
{
	while (true)
	{
		servlisten();
		acceptConnection();
		while (true)
		{
			getRequest();
			if (isExit())
				break;
			requestAPI();
		}
	}
}

void net::sendRequest(const string& reqName)
{
#if defined (_WIN32) || defined (_WIN64)
	int packet_size = send(ClientConn, package, sizeof(package), 0);

	if (packet_size == SOCKET_ERROR) {
		cout << "[" << curDateTime() << "] " << "[NET] ERROR: " << WSAGetLastError() << " Ошибка отправки сообщения клиенту." << endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		exit(1);
	}
	else
	{
		cout << "[" << curDateTime() << "] [NET] " << reqName << " send" << endl;
	}
#elif defined (__linux__)
	ssize_t bytes = write(connection, package, sizeof(package));
	if (bytes >= 0)
	{
		cout << "[" << curDateTime() << "] [NET] " << reqName << " send" << endl;
	}
#endif
}

void net::getRequest()
{
#if defined (_WIN32) || defined (_WIN64)
	bzero(package, pkg_length);
	int packet_size = recv(ClientConn, package, sizeof(package), 0);
#elif defined (__linux__)
	read(connection, package, sizeof(package));
#endif
}

bool net::isExit()
{
	if (strncmp("disconnect", package, sizeof("disconnect") - 1) == 0)
	{
		cout << "[" << curDateTime() << "] [NET] Соединение закрыто по запросу клиента." << endl;
		return true;
	}
	else
		return false;
}

void net::requestAPI()
{
	if (strncmp("GET_USRBASE", package, sizeof("GET_USRBASE") - 1) == 0)
	{
		sendUsrBase();
	}
	else if (strncmp("GET_PER_MSGBASE", package, sizeof("GET_PER_MSGBASE") - 1) == 0)
	{
		sendPerMsgBase();
	}
	else if (strncmp("GET_ALL_MSGBASE", package, sizeof("GET_ALL_MSGBASE") - 1) == 0)
	{
		sendAllMsgBase();
	}
	else if (strncmp("REG_USER", package, sizeof("REG_USER") - 1) == 0)
	{
		regUser();
	}
	else if (strncmp("SND_MSG", package, sizeof("SND_MSG") - 1) == 0)
	{
		regMSG();
	}
	else if (strncmp("DEL_USER", package, sizeof("DEL_USER") - 1) == 0)
	{
		delUsr();
	}
	else if (strncmp("CHG_PWD", package, sizeof("CHG_PWD") - 1) == 0)
	{
		chgPwd();
	}
	else if (strncmp("SET_PMSG_STATUS", package, sizeof("SET_PMSG_STATUS") - 1) == 0)
	{
		setPMStatus();
	}
	else if (strncmp("SET_AMSG_STATUS", package, sizeof("SET_AMSG_STATUS") - 1) == 0)
	{
		setAMStatus();
	}
}

void net::cutRequestHeader(string& request)
{
	request.erase(0, request.find(delim) + delim.length());
}

void net::sendUsrBase()
{
	cout << "[" << curDateTime() << "] " << "[NET] GET_USRBASE request accepted" << endl;
	if (UserBase->getCount() == 0)
	{
		db_connect->getUserBase();
	}
	for (int i = 0; i < UserBase->getCount(); i++)
	{
		bzero(package, pkg_length);
		User account = UserBase->getUser(i);
		strcpy(package, UserBase->packUsrPkg(account).data());
		reqName = "USRBASE package ";
		sendRequest(reqName.append(std::to_string(i)));
		if (i == UserBase->getCount() - 1)
		{
			bzero(package, pkg_length);
			strcpy(package, "USRBASE_END");
			reqName = "USRBASE_END";
			sendRequest(reqName);
		}
	}
}

void net::sendPerMsgBase()
{
	cout << "[" << curDateTime() << "] " << "[NET] GET_PER_MSGBASE request accepted" << endl;

	string uuid = package;
	cutRequestHeader(uuid);
	db_connect->getPerMsgBase(uuid);

	if (PerMsgBase->getCount() == 0)
	{
		bzero(package, pkg_length);
		strcpy(package, "MSGBASE_EMPTY");
		reqName = "MSGBASE_EMPTY";
		sendRequest(reqName);
		return;
	}

	for (int i = 0; i < PerMsgBase->getCount(); i++)
	{
		bzero(package, pkg_length);
		Message msg = PerMsgBase->getMsg(i);
		strcpy(package, PerMsgBase->packMsgPkg(msg).data());
		reqName = "MSGBASE package ";
		sendRequest(reqName.append(std::to_string(i)));
		if (i == PerMsgBase->getCount() - 1)
		{
			bzero(package, pkg_length);
			strcpy(package, "MSGBASE_END");
			reqName = "MSGBASE_END";
			sendRequest(reqName);
		}
	}
}

void net::sendAllMsgBase()
{
	cout << "[" << curDateTime() << "] " << "[NET] GET_ALL_MSGBASE request accepted" << endl;
	string uuid = package;
	cutRequestHeader(uuid);
	db_connect->getAllMsgBase(uuid);

	if (AllMsgBase->getCount() == 0)
	{
		bzero(package, pkg_length);
		strcpy(package, "MSGBASE_EMPTY");
		reqName = "MSGBASE_EMPTY";
		sendRequest(reqName);
		return;
	}

	for (int i = 0; i < AllMsgBase->getCount(); i++)
	{
		bzero(package, pkg_length);
		Message msg = AllMsgBase->getMsg(i);
		strcpy(package, AllMsgBase->packMsgPkg(msg).data());
		reqName = "MSGBASE package ";
		sendRequest(reqName.append(std::to_string(i)));
		if (i == AllMsgBase->getCount() - 1)
		{
			bzero(package, pkg_length);
			strcpy(package, "MSGBASE_END");
			reqName = "MSGBASE_END";
			sendRequest(reqName);
		}
	}
}

void net::regUser()
{
	cout << "[" << curDateTime() << "] " << "[NET] REG_USER request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);
	User newUser = UserBase->splitUsrPkg(temp);
	db_connect->regUser(newUser);
	db_connect->getUserBase();
	cout << "[" << curDateTime() << "] " << "[NET] User " << newUser.name << " registered" << endl;
}

void net::regMSG()
{
	cout << "[" << curDateTime() << "] " << "[NET] SND_MSG request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);

	std::thread write(&Logger::recLogEntry, logNewMsg, temp);
	std::thread read(&Logger::readLogEntry, logNewMsg);
	write.join();
	read.join();

	Message newMsg = AllMsgBase->splitMsgPkg(temp);
	if (newMsg.msgTo == "ALL")
	{
		db_connect->regAllMsg(newMsg);
		db_connect->getAllMsgBase(newMsg.msgFrom);
	}
	else
	{
		db_connect->regPrivateMsg(newMsg);
		db_connect->getPerMsgBase(newMsg.msgFrom);
	}
	cout << "[" << curDateTime() << "] " << "[NET] Message " << newMsg.msgText << " send" << endl;
}

void net::delUsr()
{
	cout << "[" << curDateTime() << "] " << "[NET] DEL_USER request accepted" << endl;
	string uuid = package;
	cutRequestHeader(uuid);
	db_connect->delUser(uuid);
	db_connect->getUserBase();
	cout << "[" << curDateTime() << "] " << "[NET] User with UUID " << uuid << " deleted." << endl;
}

void net::chgPwd()
{
	cout << "[" << curDateTime() << "] " << "[NET] CHG_PWD request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);
	string arr[2];
	UserBase->splitChgPwd(temp, arr);
	db_connect->chgPwd(arr[0], arr[1]);
	db_connect->getUserBase();
	cout << "[" << curDateTime() << "] " << "[NET] Password for user " << UserBase->getUser(arr[0]).name << " changed" << endl;
}

void net::setPMStatus()
{
	cout << "[" << curDateTime() << "] " << "[NET] SET_PMSG_STATUS request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);
	int arr[2];
	AllMsgBase->splitPMStatus(temp, arr);
	db_connect->updPMStatus(arr[0], arr[1]);
}

void net::setAMStatus()
{
	cout << "[" << curDateTime() << "] " << "[NET] SET_PMSG_STATUS request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);
	string arr[3];
	AllMsgBase->splitAMStatus(temp, arr);
	db_connect->updAMStatus(arr[0], arr[1], arr[2]);
}