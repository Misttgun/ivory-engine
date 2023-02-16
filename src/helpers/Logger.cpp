#include "Logger.h"
#include <chrono>
#include <ctime>
#include <iostream>

#pragma warning(disable:4996)

std::vector<LogEntry> Logger::m_logMessages;

void Logger::Log(const std::string& message)
{
	const std::string output = "LOG: [" + GetCurrentDateTimeToString() + "]: " + message;
	std::cout << "\x1B[32m" << output << "\033[0m" << std::endl;
	m_logMessages.push_back(LogEntry(LOG_INFO, output));
}

void Logger::LogWarning(const std::string & message)
{
	const std::string output = "WRN: [" + GetCurrentDateTimeToString() + "]: " + message;
	std::cout << "\x1B[93m" << output << "\033[0m" << std::endl;
	m_logMessages.push_back(LogEntry(LOG_WARNING, output));
}

void Logger::LogError(const std::string & message)
{
	const std::string output = "ERR: [" + GetCurrentDateTimeToString() + "]: " + message;
	std::cout << "\x1B[91m" << output << "\033[0m" << std::endl;
	m_logMessages.push_back(LogEntry(LOG_ERROR, output));
}

std::string Logger::GetCurrentDateTimeToString()
{
	const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string output(30, '\0');
	std::strftime(output.data(), output.size(), "%d-%b-%Y %H:%M:%S", std::localtime(&now));
	return output;
}
