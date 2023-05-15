#include "Timer.h"

namespace Ivory
{
	void Timer::Start()
	{
		m_startTime = std::chrono::system_clock::now();
		m_running = true;
	}

	void Timer::Stop()
	{
		m_endTime = std::chrono::system_clock::now();
		m_running = false;
	}

	double Timer::ElapsedMilliseconds() const
	{
		std::chrono::time_point<std::chrono::system_clock> endTime;

		if (m_running)
			endTime = std::chrono::system_clock::now();
		else
			endTime = m_endTime;

		return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime).count());
	}

	double Timer::ElapsedSeconds() const
	{
		return ElapsedMilliseconds() / 1000.0;
	}
}
