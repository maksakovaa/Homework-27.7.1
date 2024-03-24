#include "Logger.h"
Logger::Logger()
{
	slog.open(log_file, std::ios::in | std::ios::app);
}

Logger::~Logger()
{
	slog.close();
}

void Logger::recLogEntry(const string& entry)
{
	s_mutex.lock();
	slog << entry;
	slog << "\n";
	s_mutex.unlock();
}

void Logger::readLogEntry()
{
	std::this_thread::sleep_for(std::chrono::microseconds(100));
	s_mutex.lock_shared();
	slog.seekg(diff, std::ios_base::end);
	slog.seekg(diff, std::ios_base::cur);
	for (long long int i = slog.tellg(); i >= 0; --i)
	{
		if (slog.peek() == '\n')
		{
			slog.get();
			break;
		}
		slog.seekg(i, std::ios_base::beg);
	}
	getline(slog, logEntry);
	cout << logEntry << endl;
	s_mutex.unlock_shared();
}