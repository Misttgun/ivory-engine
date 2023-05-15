#pragma once

#include <chrono>

namespace Ivory
{
	class Timer
	{
	public:
		void Start();
		void Stop();
		[[nodiscard]] double ElapsedMilliseconds() const;
		[[nodiscard]] double ElapsedSeconds() const;

	private:
		std::chrono::time_point<std::chrono::system_clock> m_startTime;
		std::chrono::time_point<std::chrono::system_clock> m_endTime;
		bool m_running = false;
	};
}
