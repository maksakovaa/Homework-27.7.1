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

	if (erStat <= 0)
	{
		logging("[NET] ERROR: Ошибка преобразования IP адреса в специальный цифровой формат.");
//		cout << logTimeStamp() << "[NET] ERROR: Ошибка преобразования IP адреса в специальный цифровой формат." << endl;
		exit(1);
	}
}

void net::initWinsock()
{
	if ((erStat = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		logging("[NET] ERROR: Ошибка инициализации WSAStartup");
//		cout << logTimeStamp() << "[NET] ERROR: Ошибка инициализации WSAStartup" << erStat;
		exit(1);
	}

	else
	{
		logging("[NET] WSAStartup: " + string(wsaData.szSystemStatus));
//		cout << logTimeStamp() << "[NET] WSAStartup: " << wsaData.szSystemStatus << endl;
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

	if (ServSock == INVALID_SOCKET)
	{
		logging("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка создания сокета.");
//		cout << logTimeStamp() << "[NET] ERROR: " << WSAGetLastError() << " Ошибка создания сокета." << endl;
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] Инициализация сокета успешна.");
//		cout << logTimeStamp() << "[NET] Инициализация сокета успешна." << endl;
	}
#elif defined (__linux__)
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		logging("[NET] ERROR: Ошибка создания сокета.");
//		cout << logTimeStamp() << "[NET] ERROR: Ошибка создания сокета." << endl;
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
	if (erStat != 0)
	{
		logging("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка привязки сокета");
//		cout << logTimeStamp() << "[NET] ERROR: " << WSAGetLastError() << " Ошибка привязки сокета # " << endl;
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] Привязка сокета выполнена.");
//		cout << logTimeStamp() << "[NET] Привязка сокета выполнена." << endl;
	}
#elif defined (__linux__)
	srvaddress.sin_addr.s_addr = htonl(INADDR_ANY);
	srvaddress.sin_port = htons(std::stoi(chat_port));
	srvaddress.sin_family = AF_INET;

	erStat = bind(socket_fd, (struct sockaddr*)&srvaddress, sizeof(srvaddress));
	if (erStat == -1)
	{
		logging("[NET] ERROR: Ошибка привязки сокета.");
//		cout << logTimeStamp() << "[NET] ERROR: Ошибка привязки сокета." << endl;
		exit(1);
	}
	else
	{
		logging("[NET] Привязка сокета выполнена.");
//		cout << logTimeStamp() << "[NET] Привязка сокета выполнена." << endl;
	}
		
#endif
}

void net::servlisten()
{
#if defined (_WIN32) || defined (_WIN64)
	erStat = listen(ServSock, SOMAXCONN);

	if (erStat != 0)
	{
		logging("[NET] ERROR: Ошибка при постановке на приём данных # " + std::to_string(WSAGetLastError()));
//		cout << logTimeStamp() << "[NET] ERROR: Ошибка при постановке на приём данных # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] Ожидание подключений...");
//		cout << logTimeStamp() << "[NET] Ожидание подключений..." << endl;
	}
#elif defined (__linux__)
	erStat = listen(socket_fd, 20);
	if (erStat == -1)
	{
		logging("[NET] ERROR: Ошибка при постановке на приём данных.");
//		cout << logTimeStamp() << "[NET] ERROR: Ошибка при постановке на приём данных." << endl;
		exit(1);
	}
	else
	{
		logging("[NET] Ожидание подключений...");
//		cout << logTimeStamp() << "[NET] Ожидание подключений..." << endl;
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

	if (ClientConn == INVALID_SOCKET)
	{
		logging("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Сервер несмог принять данные от клиента.");
//		cout << logTimeStamp() << "[NET] ERROR: " << WSAGetLastError() << " Сервер несмог принять данные от клиента." << endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] Соеединение успешно установлено");
//		cout << logTimeStamp() << "[NET] Соеединение успешно установлено" << endl;
	}
#elif defined (__linux__)
	length = sizeof(client);
	connection = accept(socket_fd, (struct sockaddr*)&client, &length);
	if (connection == -1)
	{
		logging("[NET] ERROR: Сервер несмог принять данные от клиента.");
//		cout << logTimeStamp() << "[NET] ERROR: Сервер несмог принять данные от клиента." << endl;
		exit(1);
	}
	else
	{
		logging("[NET] Соеединение успешно установлено");
//		cout << logTimeStamp() << "[NET] Соеединение успешно установлено" << endl;
	}
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

	if (packet_size == SOCKET_ERROR)
	{
		logging("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка отправки сообщения клиенту.");
//		cout << logTimeStamp() << "[NET] ERROR: " << WSAGetLastError() << " Ошибка отправки сообщения клиенту." << endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] " + reqName + " send");
//		cout << logTimeStamp() << "[NET] " << reqName << " send" << endl;
	}
#elif defined (__linux__)
	ssize_t bytes = write(connection, package, sizeof(package));
	if (bytes >= 0)
	{
		logging("[NET] " + reqName + " send");
//		cout << logTimeStamp() << "[NET] " << reqName << " send" << endl;
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
		logging("[NET] Соединение закрыто по запросу клиента.");
//		cout << logTimeStamp() << "[NET] Соединение закрыто по запросу клиента." << endl;
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
	logging("[NET] GET_USRBASE request accepted");
//	cout << logTimeStamp() << "[NET] GET_USRBASE request accepted" << endl;
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
	logging("[NET] GET_PER_MSGBASE request accepted");
//	cout << logTimeStamp() << "[NET] GET_PER_MSGBASE request accepted" << endl;

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
	logging("[NET] GET_ALL_MSGBASE request accepted");
//	cout << logTimeStamp() << "[NET] GET_ALL_MSGBASE request accepted" << endl;
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
	logging("[NET] REG_USER request accepted");
//	cout << logTimeStamp() << "[NET] REG_USER request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);
	User newUser = UserBase->splitUsrPkg(temp);
	db_connect->regUser(newUser);
	db_connect->getUserBase();
	logging("[NET] User \"" + newUser.name + "\" registered");
//	cout << logTimeStamp() << "[NET] User " << newUser.name << " registered" << endl;
}

void net::regMSG()
{
	logging("[NET] SND_MSG request accepted");
//	cout << logTimeStamp() << "[NET] SND_MSG request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);

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
	logging("[NET] Message \"" + newMsg.msgText + "\" send");
}

void net::delUsr()
{
	logging("[NET] DEL_USER request accepted");
//	cout << logTimeStamp() << "[NET] DEL_USER request accepted" << endl;
	string uuid = package;
	cutRequestHeader(uuid);
	db_connect->delUser(uuid);
	db_connect->getUserBase();
	logging("[NET] User with UUID \"" + uuid + "\" deleted.");
//	cout << logTimeStamp() << "[NET] User with UUID " << uuid << " deleted." << endl;
}

void net::chgPwd()
{
	logging("[NET] CHG_PWD request accepted");
//	cout << logTimeStamp() << "[NET] CHG_PWD request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);
	string arr[2];
	UserBase->splitChgPwd(temp, arr);
	db_connect->chgPwd(arr[0], arr[1]);
	db_connect->getUserBase();
	logging("[NET] Password for user \"" + UserBase->getUser(arr[0]).name + "\" changed");
//	cout << logTimeStamp() << "[NET] Password for user " << UserBase->getUser(arr[0]).name << " changed" << endl;
}

void net::setPMStatus()
{
	logging("[NET] SET_PMSG_STATUS request accepted");
//	cout << logTimeStamp() << "[NET] SET_PMSG_STATUS request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);
	int arr[2];
	AllMsgBase->splitPMStatus(temp, arr);
	db_connect->updPMStatus(arr[0], arr[1]);
}

void net::setAMStatus()
{
	logging("[NET] SET_PMSG_STATUS request accepted");
//	cout << logTimeStamp() << "[NET] SET_PMSG_STATUS request accepted" << endl;
	string temp = package;
	cutRequestHeader(temp);
	string arr[3];
	AllMsgBase->splitAMStatus(temp, arr);
	db_connect->updAMStatus(arr[0], arr[1], arr[2]);
}

void net::logging(const string& entry)
{
	std::thread write(&Logger::recLogEntry, Log, std::cref(entry));
	std::thread read(&Logger::readLogEntry, Log);
	write.join();
	read.join();
}
