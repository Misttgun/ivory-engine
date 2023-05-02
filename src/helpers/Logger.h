#pragma once

#include <string>
#include <vector>

namespace re::utils
{
	enum class LogType
	{
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR
	};

	struct LogEntry
	{
		LogType m_type;
		std::string m_message;
	};

	class Logger
	{
	public:
		static std::vector<LogEntry> m_logMessages;

		static void Log(const std::string& message);
		static void LogWarning(const std::string& message);
		static void LogError(const std::string& message);

	private:
		static std::string GetCurrentDateTimeToString();
	};
}
