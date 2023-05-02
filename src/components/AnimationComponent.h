#pragma once

namespace re
{
	struct AnimationComponent
	{
		int32 m_frames;
		int32 m_currentFrame;
		int32 m_speed;
		bool m_shouldLoop;
		float m_frameTime;

		AnimationComponent() = default;

		explicit AnimationComponent(const int32 frames, const int32 speed, const bool shouldLoop) : m_frames(frames), m_speed(speed), m_shouldLoop(shouldLoop)
		{
			m_currentFrame = 1;
			m_frameTime = 0.0f;
		}
	};
}
