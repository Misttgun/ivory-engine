#pragma once

#include <chrono>

namespace re::utils
{
	class Timer
	{
	public:
		void Start();
		void Stop();
		double ElapsedMilliseconds() const;
		double ElapsedSeconds() const;

	private:
		std::chrono::time_point<std::chrono::system_clock> m_startTime;
		std::chrono::time_point<std::chrono::system_clock> m_endTime;
		bool m_running = false;
	};
}
