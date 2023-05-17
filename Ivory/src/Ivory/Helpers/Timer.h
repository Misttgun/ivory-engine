#pragma once
#include "Ivory/Types.h"

namespace Ivory
{
	class Timer
	{
	public:
		void Start();
		void Stop();
		[[nodiscard]] float ElapsedMilliseconds() const;
		[[nodiscard]] float ElapsedSeconds() const;

	private:
		uint64 m_start = 0;
		uint64 m_end = 0;
		bool m_running = false;
	};
}
