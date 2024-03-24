#include "sql_db.h"

SQL_DB::SQL_DB()
{
	getConfig();
	mysql_init(&mysql);
	if (&mysql == NULL)
	{
		logging(logTimeStamp() + "[MySQL] ERROR: Ошибка создания MySQL дескриптора");
//		cout << logTimeStamp() << "[MySQL] ERROR: Ошибка создания MySQL дескриптора" << endl;
		exit(1);
	}

	if (!mysql_real_connect(&mysql, mysql_srv_ip.data(), mysql_login.data(), mysql_pass.data(), NULL, 0, NULL, 0))
	{
		logging(logTimeStamp() + "[MySQL] ERROR: Ошибка подключения к базе данных " + mysql_error(&mysql));
//		cout << logTimeStamp() << "[MySQL] ERROR: Ошибка подключения к базе данных " << mysql_error(&mysql) << endl;
		exit(1);
	}
	else
	{
		logging(logTimeStamp() + "[MySQL] Соединение установлено.");
//		cout << logTimeStamp() << "[MySQL] Соединение установлено." << endl;
	}
	mysql_set_character_set(&mysql, "utf8");
	firstRun();
}

SQL_DB::~SQL_DB()
{
	mysql_close(&mysql);
	logging(logTimeStamp() + "[MySQL] Соединение закрыто.");
//	cout << logTimeStamp() << "[MySQL] Соединение закрыто." << endl;
}

void SQL_DB::getConfig()
{
	mysql_srv_ip = Config->getMysqlSrvIP();
	mysql_login = Config->getMysqlLogin();
	mysql_pass = Config->getMysqlPass();
	mysql_database = Config->getMysql_database();
	mysql_table_users = Config->getMysql_table_users();
	mysql_table_AM = Config->getMysql_table_AM();
	mysql_table_AMS = Config->getMysql_table_AMS();
	mysql_table_PM = Config->getMysql_table_PM();
}

void SQL_DB::firstRun()
{
	m_query = "CREATE DATABASE IF NOT EXISTS " + mysql_database;
	sendRequest("CREATE DATABASE");

	m_query = "USE " + mysql_database;
	sendRequest("USE DATABASE");

	m_query = "CREATE TABLE IF NOT EXISTS " + mysql_table_users + " ";
	m_query += "(id SERIAL NOT NULL,";
	m_query += "uuid BINARY(16) PRIMARY KEY DEFAULT (UUID_TO_BIN(UUID())), ";
	m_query += "name VARCHAR(100) NOT NULL, ";
	m_query += "login VARCHAR(100) UNIQUE NOT NULL, ";
	m_query += "pwd VARCHAR(200), ";
	m_query += "deleted INTEGER NOT NULL DEFAULT 0)";
	sendRequest("CREATE TABLE USERS");

	m_query = "CREATE TABLE IF NOT EXISTS " + mysql_table_AM;
	m_query += " (id SERIAL PRIMARY KEY, ";
	m_query += "msg_from BINARY(16) NOT NULL REFERENCES " + mysql_table_users + "(uuid), ";
	m_query += "date DATETIME NOT NULL, ";
	m_query += "message VARCHAR(255) NOT NULL)";
	sendRequest("CREATE TABLE ALL_MSGBASE");

	m_query = "CREATE TABLE IF NOT EXISTS " + mysql_table_PM;
	m_query += " (id SERIAL PRIMARY KEY, ";
	m_query += "msg_from BINARY(16) NOT NULL REFERENCES " + mysql_table_users + "(uuid), ";
	m_query += "msg_to BINARY(16) NOT NULL REFERENCES " + mysql_table_users + "(uuid), ";
	m_query += "date DATETIME NOT NULL, ";
	m_query += "message VARCHAR(255) NOT NULL, ";
	m_query += "status INTEGER DEFAULT 0)";
	sendRequest("CREATE TABLE PRIVATE_MSGBASE");

	m_query += "CREATE TABLE IF NOT EXISTS " + mysql_table_AMS + " ";
	m_query += "(id SERIAL PRIMARY KEY, ";
	m_query += "msg_id integer NOT NULL REFERENCES " + mysql_table_AM + "(id), ";
	m_query += "for_user BINARY(16) NOT NULL REFERENCES " + mysql_table_users + "(uuid), ";
	m_query += "status INTEGER DEFAULT 0)";
	sendRequest("CREATE TABLE ALL MSG STATUS");

	m_query += "CREATE PROCEDURE IF NOT EXISTS status_on_create_msg(in a INT) ";
	m_query += "BEGIN ";
	m_query += "DECLARE i INT DEFAULT 1; ";
	m_query += "DECLARE user_count INT DEFAULT 0; ";
	m_query += "DECLARE user_uuid BINARY(16); ";
	m_query += "SET user_count = (SELECT count(u.id) FROM " + mysql_table_users + " u); ";
	m_query += "WHILE i <= user_count DO ";
	m_query += "SET user_uuid = (SELECT u.uuid FROM " + mysql_table_users + " u WHERE u.id = i); ";
	m_query += "INSERT INTO " + mysql_table_AMS + " (msg_id, for_user) VALUES (a, user_uuid); ";
	m_query += "SET i = i+1; ";
	m_query += "END WHILE; ";
	m_query += "END;";
	sendRequest("CREATE PROCEDURE status_on_create_msg");

	m_query += "CREATE TRIGGER IF NOT EXISTS on_create_msg AFTER INSERT ON " + mysql_table_AM + " ";
	m_query += "FOR EACH ROW ";
	m_query += "BEGIN ";
	m_query += "CALL status_on_create_msg( NEW.id ); ";
	m_query += "END;";
	sendRequest("CREATE TRIGGER on_create_msg");

	m_query += "CREATE PROCEDURE IF NOT EXISTS status_on_create_usr(in new_uuid BINARY(16)) ";
	m_query += "BEGIN ";
	m_query += "DECLARE i INT DEFAULT 1; ";
	m_query += "DECLARE msg_count INT DEFAULT 0; ";
	m_query += "SET msg_count = (SELECT count(id) FROM " + mysql_table_AM + "); ";
	m_query += "IF msg_count >= 1 THEN ";
	m_query += "WHILE i <= msg_count DO ";
	m_query += "INSERT INTO " + mysql_table_AMS + " (msg_id, for_user) VALUES (i, new_uuid); ";
	m_query += "SET i = i+1; ";
	m_query += "END WHILE; ";
	m_query += "END IF; ";
	m_query += "END;";
	sendRequest("CREATE PROCEDURE status_on_create_usr");

	m_query += "CREATE TRIGGER IF NOT EXISTS on_create_usr AFTER INSERT ON " + mysql_table_users + " ";
	m_query += "FOR EACH ROW ";
	m_query += "BEGIN ";
	m_query += "CALL status_on_create_usr( NEW.uuid ); ";
	m_query += "END;";
	sendRequest("CREATE TRIGGER on_create_usr");

	getUserBase();
}

void SQL_DB::getRequest(const string& request)
{
	mysql_query(&mysql, m_query.data());
	if (result = mysql_store_result(&mysql))
	{
		logging(logTimeStamp() + "[MySQL] Запрос " + request + " выполнен успешно.");
//		cout << logTimeStamp() << "[MySQL] Запрос " << request << " выполнен успешно." << endl;
	}
	else
	{
		logging(logTimeStamp() + "[MySQL] Ошибка MySQL " + mysql_error(&mysql));
//		cout << logTimeStamp() << "[MySQL] Ошибка MySQL " << mysql_error(&mysql) << endl;
	}
	m_query.clear();
}

void SQL_DB::sendRequest(const string& request)
{
	int query = mysql_query(&mysql, m_query.data());
	if (query == 0)
	{
		logging(logTimeStamp() + "[MySQL] Запрос " + request + " выполнен успешно.");
//		cout << logTimeStamp() << "[MySQL] Запрос " << request << " выполнен успешно." << endl;
	}
	else
	{
		logging(logTimeStamp() + "[MySQL] Запрос " + request + " выполнен c ошибкой: " + std::to_string(query));
//		cout << logTimeStamp() << "[MySQL] Запрос " << request << " выполнен c ошибкой: " << query << endl;
	}
	m_query.clear();
}

void SQL_DB::getUserBase()
{
	UserBase->resetBase();
	m_query = "SELECT BIN_TO_UUID(uuid) uuid, name, login, pwd, deleted  FROM " + mysql_table_users + " ORDER BY id";
	getRequest("SELECT FOR USERBASE");
	while (row = mysql_fetch_row(result))
	{
		User newUser(row[0], row[1], row[2], row[3], atoi(row[4]));
		UserBase->addInBase(newUser);
		logging(logTimeStamp() + "[MySQL] Пользователь \"" + newUser.name + "\" добавлен в базу");
//		cout << logTimeStamp() << "[MySQL] Пользователь " << newUser.name << " добавлен в базу" << endl;
	}
	if (UserBase->getCount() == 0)
	{
		User newUser("Администратор", "admin", "da23890e111536e631be13a069ebc5432c9cf28cdbc5deb2a70770ec9597db6d");
		regUser(newUser);
	}
}

void SQL_DB::getPerMsgBase(const string& uuid)
{
	PerMsgBase->resetBase();

	m_query += "SELECT pm.id, BIN_TO_UUID(pm.msg_from), ";
	m_query += "BIN_TO_UUID(pm.msg_to), pm.date, pm.message, pm.status ";
	m_query += "FROM " + mysql_table_PM + " pm ";
	m_query += "WHERE pm.msg_from = UUID_TO_BIN('" + uuid + "') OR pm.msg_to = UUID_TO_BIN('" + uuid + "') ";
	m_query += "ORDER BY pm.id";
	getRequest("SELECT FOR PRIVATE_MSGBASE");
	while (row = mysql_fetch_row(result))
	{
		Message newMsg(atoi(row[0]), row[1], row[2], row[3], row[4], atoi(row[5]));
		PerMsgBase->addInBase(newMsg);
	}
}

void SQL_DB::getAllMsgBase(const string& uuid)
{
	AllMsgBase->resetBase();
	m_query += "SELECT m.id, BIN_TO_UUID(m.msg_from), m.date, m.message, ams.status ";
	m_query += "FROM " + mysql_table_AM + " m ";
	m_query += "JOIN " + mysql_table_AMS + " ams ON m.id = ams.msg_id ";
	m_query += "WHERE ams.for_user = UUID_TO_BIN('" + uuid + "') ";
	m_query += "ORDER BY m.id";
	getRequest("SELECT FOR ALL_MSGBASE");
	while (row = mysql_fetch_row(result))
	{
		Message newMsg(atoi(row[0]), row[1], "ALL", row[2], row[3], atoi(row[4]));
		AllMsgBase->addInBase(newMsg);
	}
}

void SQL_DB::delUser(const string& uuid)
{
	m_query += "UPDATE " + mysql_table_users + " ";
	m_query += "SET deleted = 1 ";
	m_query += "WHERE BIN_TO_UUID(uuid) = '" + uuid + "'";
	sendRequest("DELETE FOR USER");
}

void SQL_DB::regUser(User& newUser)
{
	m_query += "INSERT INTO " + mysql_table_users + " ";
	m_query += "(name, login, pwd) ";
	m_query += "VALUES('" + newUser.name + "', '" + newUser.login + "', '" + newUser.pwd + "')";
	sendRequest("INSERT USER IN USERBASE");
}

void SQL_DB::chgPwd(const string& uuid, const string& pwd)
{
	m_query += "UPDATE " + mysql_table_users + " ";
	m_query += "SET pwd = '" + pwd + "'";
	m_query += "WHERE uuid = UUID_TO_BIN('" + uuid + "')";
	sendRequest("UPDATE PWD FOR USER");
}

void SQL_DB::regPrivateMsg(Message& newMsg)
{
	m_query += "INSERT INTO " + mysql_table_PM + " ";
	m_query += "(msg_from, msg_to, date, message) ";
	m_query += "VALUES(UUID_TO_BIN('" + newMsg.msgFrom + "'), UUID_TO_BIN('" + newMsg.msgTo + "'), '" + newMsg.msgDate + "', '" + newMsg.msgText + "')";
	sendRequest("INSERT MSG IN PRIVATE_MSGBASE");
}

void SQL_DB::regAllMsg(Message& newMsg)
{
	m_query += "INSERT INTO " + mysql_table_AM + " ";
	m_query += "(msg_from, date, message) ";
	m_query += "VALUES(UUID_TO_BIN('" + newMsg.msgFrom + "'), '" + newMsg.msgDate + "', '" + newMsg.msgText + "')";
	sendRequest("INSERT MSG IN ALL_MSGBASE");
}

void SQL_DB::updPMStatus(int msgId, int status)
{
	m_query += "UPDATE " + mysql_table_PM + " SET status = " + std::to_string(status) + " WHERE id = " + std::to_string(msgId);
	sendRequest("UPDATE PRIVATE MSG status");
}

void SQL_DB::updAMStatus(const string& msgId, const string& forUser, const string& status)
{
	m_query += "UPDATE " + mysql_table_AMS + " SET status = " + status + " WHERE msg_id = " + msgId + " AND for_user = UUID_TO_BIN('" + forUser + "')";
	sendRequest("UPDATE ALL MSG status");
}

void SQL_DB::logging(const string& entry)
{
	std::thread write(&Logger::recLogEntry, Log, std::cref(entry));
	std::thread read(&Logger::readLogEntry, Log);
	write.join();
	read.join();
}
