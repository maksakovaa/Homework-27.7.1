#include "settings.h"

string curDateTime()
{
	time_t curtime;
	struct tm* loctime;
	char buffer[26];
	time(&curtime);
	loctime = localtime(&curtime);
	strftime(buffer, 26, "%Y.%m.%d %H:%M:%S", loctime);
	std::string result = buffer;
	return result;
}

Settings::Settings()
{
#if defined (_WIN32) || defined (_WIN64)
	SetConsoleOutputCP(65001);
	SetConsoleCP(1251);
#elif defined (__linux__)
	setlocale(LC_ALL, "ru_RU.utf8");
#endif
	std::ifstream config(cfgPath);
	if (config.is_open())
	{
		string str;
		while (std::getline(config, str))
		{
			if (strncmp("mysql_server_ip", str.data(), (int)strlen("mysql_server_ip")) == 0)
			{
				mysql_srv_ip = getValue(str);
			}
			else if (strncmp("mysql_login", str.data(), (int)strlen("mysql_login")) == 0)
			{
				mysql_login = getValue(str);
			}
			else if (strncmp("mysql_pass", str.data(), (int)strlen("mysql_pass")) == 0)
			{
				mysql_pass = getValue(str);
			}
			else if (strncmp("mysql_database", str.data(), (int)strlen("mysql_database")) == 0)
			{
				mysql_database = getValue(str);
			}
			else if (strncmp("mysql_table_users", str.data(), (int)strlen("mysql_table_users")) == 0)
			{
				mysql_table_users = getValue(str);
			}
			else if (strncmp("mysql_table_messages", str.data(), (int)strlen("mysql_table_messages")) == 0)
			{
				mysql_table_AM = getValue(str);
			}
			else if (strncmp("mysql_table_all_msg_status", str.data(), (int)strlen("mysql_table_all_msg_status")) == 0)
			{
				mysql_table_AMS = getValue(str);
			}
			else if (strncmp("mysql_table_private_msg", str.data(), (int)strlen("mysql_table_private_msg")) == 0)
			{
				mysql_table_PM = getValue(str);
			}
			else if (strncmp("server_ip", str.data(), (int)strlen("server_ip")) == 0)
			{
				server_ip = getValue(str);
			}
			else if (strncmp("chat_port", str.data(), (int)strlen("chat_port")) == 0)
			{
				chat_port = getValue(str);
			}
		}
		config.close();
	}
	else
	{
		mysql_srv_ip = "127.0.0.1";
		mysql_login = "root";
		mysql_pass = "YrZBQJIijJ9W0mlpqrj4";
		mysql_database = "console_chat";
		mysql_table_users = "users";
		mysql_table_AM = "messages";
		mysql_table_AMS = "all_msg_status";
		mysql_table_PM = "p_messages";
		server_ip = "127.0.0.1";
		chat_port = "9999";
		saveConfig();
	}
}

Settings::~Settings()
{
	saveConfig();
}

const string& Settings::getMysqlSrvIP()
{
	return mysql_srv_ip;
}

const string& Settings::getMysqlLogin()
{
	return mysql_login;
}

const string& Settings::getMysqlPass()
{
	return mysql_pass;
}

const string& Settings::getServerIP()
{
	return server_ip;
}

const string& Settings::getChatPort()
{
	return chat_port;
}

const string& Settings::getMysql_database()
{
	return mysql_database;
}

const string& Settings::getMysql_table_users()
{
	return mysql_table_users;
}

const string& Settings::getMysql_table_AM()
{
	return mysql_table_AM;
}

const string& Settings::getMysql_table_AMS()
{
	return mysql_table_AMS;
}

const string& Settings::getMysql_table_PM()
{
	return mysql_table_PM;
}

void Settings::saveConfig()
{
	std::ofstream config(cfgPath, std::ios::trunc);
	if (!config.is_open())
	{
		cout << "[" << curDateTime() << "] ERROR: Ошибка открытия файла!" << endl;
	}
	else
	{
		config << ("mysql_server_ip = " + mysql_srv_ip + "\n");
		config << ("mysql_login = " + mysql_login + "\n");
		config << ("mysql_pass = " + mysql_pass + "\n");
		config << ("mysql_database = " + mysql_database + "\n");
		config << ("mysql_table_users = " + mysql_table_users + "\n");
		config << ("mysql_table_messages = " + mysql_table_AM + "\n");
		config << ("mysql_table_all_msg_status = " + mysql_table_AMS + "\n");
		config << ("mysql_table_private_msg = " + mysql_table_PM + "\n");
		config << ("server_ip = " + server_ip + "\n");
		config << ("chat_port = " + chat_port + "\n");
		config.close();
	}
}

string& Settings::getValue(string& str)
{
	return str.erase(0, str.find(delim_settings) + delim_settings.length());
}
