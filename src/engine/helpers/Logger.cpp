#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iostream>

#pragma warning(disable:4996)

namespace re::utils
{
	std::vector<LogEntry> Logger::m_logMessages;

	void Logger::Log(const std::string& message)
	{
		const std::string output = "INFO: [" + GetCurrentDateTimeToString() + "]: " + message;
		std::cout << "\x1B[32m" << output << "\033[0m" << '\n';
		m_logMessages.push_back(LogEntry(LogType::LOG_WARNING, output));
	}

	void Logger::LogWarning(const std::string& message)
	{
		const std::string output = "WRN: [" + GetCurrentDateTimeToString() + "]: " + message;
		std::cout << "\x1B[93m" << output << "\033[0m" << '\n';
		m_logMessages.push_back(LogEntry(LogType::LOG_WARNING, output));
	}

	void Logger::LogError(const std::string& message)
	{
		const std::string output = "ERR: [" + GetCurrentDateTimeToString() + "]: " + message;
		std::cout << "\x1B[91m" << output << "\033[0m" << '\n';
		m_logMessages.push_back(LogEntry(LogType::LOG_ERROR, output));
	}

	std::string Logger::GetCurrentDateTimeToString()
	{
		const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string output(30, '\0');

		tm newtTime{};
		[[maybe_unused]] errno_t success = localtime_s(&newtTime, &now);

		[[maybe_unused]] size_t length = std::strftime(output.data(), output.size(), "%d-%m-%Y %H:%M:%S", &newtTime);

		return output;
	}
}
