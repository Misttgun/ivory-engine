#pragma once

struct ProjectileComponent
{
	bool m_isFriendly;
	int m_damage;
	float m_duration;
	float m_durationTimer;

	ProjectileComponent() = default;

	ProjectileComponent(const bool isFriendly, const int damage, const float duration) : m_isFriendly(isFriendly), m_damage(damage), m_duration(duration)
	{
		m_durationTimer = 0;
	}
};
