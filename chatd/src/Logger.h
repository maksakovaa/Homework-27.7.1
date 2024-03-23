#pragma once
#include "sha256d.h"
#include <thread>
#include <shared_mutex>
class Logger
{
public:
	Logger();
	~Logger();
	void recLogEntry(const string& entry);
	void readLogEntry();
private:
#if defined (_WIN32) || defined (_WIN64)
	string log_file = "log.txt";
	int diff = -2;
#elif defined (__linux__)
	string log_file = "/var/log/chatd/log.txt";
	int diff = -1;
#endif
	std::fstream log;
	std::shared_mutex s_mutex;
	string logEntry = "";
};