#pragma once

struct AnimationComponent
{
	int m_frames;
	int m_currentFrame;
	int m_speed;
	bool m_shouldLoop;
	float m_frameTime;

	AnimationComponent() = default;

	explicit AnimationComponent(const int frames, const int speed, const bool shouldLoop) : m_frames(frames), m_speed(speed), m_shouldLoop(shouldLoop)
	{
		m_currentFrame = 1;
		m_frameTime = 0.0f;
	}
};