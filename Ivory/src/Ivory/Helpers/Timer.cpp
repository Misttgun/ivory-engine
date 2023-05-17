#include "Timer.h"

#include <SDL_timer.h>

namespace Ivory
{
	void Timer::Start()
	{
		m_start = SDL_GetPerformanceCounter();
		m_running = true;
	}

	void Timer::Stop()
	{
		m_end = SDL_GetPerformanceCounter();
		m_running = false;
	}

	float Timer::ElapsedMilliseconds() const
	{
		return ElapsedSeconds() * 1000.0f;
	}

	float Timer::ElapsedSeconds() const
	{
		uint64 end;

		if (m_running)
			end = SDL_GetPerformanceCounter();
		else
			end = m_end;

		return static_cast<float>(end - m_start) / static_cast<float>(SDL_GetPerformanceFrequency());
	}
}
