#include "Logger.h"
Logger::Logger()
{
	log.open(log_file, std::ios::in | std::ios::app);
}

Logger::~Logger()
{
	log.close();
}

void Logger::recLogEntry(const string& entry)
{
	s_mutex.lock();
	log << entry;
	log << "\n";
	s_mutex.unlock();
}

void Logger::readLogEntry()
{
	s_mutex.lock_shared();
	log.seekg(diff, std::ios_base::end);
	log.seekg(diff, std::ios_base::cur);
	for (long long int i = log.tellg(); i >= 0; --i)
	{
		if (log.peek() == '\n')
		{
			log.get();
			break;
		}
		log.seekg(i, std::ios_base::beg);
	}
	getline(log, logEntry);
	cout << logEntry << endl;
	s_mutex.unlock_shared();
}