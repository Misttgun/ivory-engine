#pragma once

#include <memory>

#include "spdlog/spdlog.h"

namespace Ivory
{
	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;}
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger;}

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define IV_CORE_TRACE(...)	::Ivory::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define IV_CORE_INFO(...)	::Ivory::Log::GetCoreLogger()->info(__VA_ARGS__)
#define IV_CORE_WARN(...)	::Ivory::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define IV_CORE_ERROR(...)	::Ivory::Log::GetCoreLogger()->error(__VA_ARGS__)
#define IV_CORE_FATAL(...)	::Ivory::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define IV_TRACE(...)		::Ivory::Log::GetClientLogger()->trace(__VA_ARGS__)
#define IV_INFO(...)		::Ivory::Log::GetClientLogger()->info(__VA_ARGS__)
#define IV_WARN(...)		::Ivory::Log::GetClientLogger()->warn(__VA_ARGS__)
#define IV_ERROR(...)		::Ivory::Log::GetClientLogger()->error(__VA_ARGS__)
#define IV_FATAL(...)		::Ivory::Log::GetClientLogger()->fatal(__VA_ARGS__)