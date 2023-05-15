#pragma once

namespace Ivory
{
	struct ProjectileComponent
	{
		bool m_isFriendly;
		int32 m_damage;
		float m_duration;
		float m_durationTimer;

		ProjectileComponent() = default;

		explicit ProjectileComponent(const bool isFriendly, const int32 damage, const float duration) : m_isFriendly(isFriendly), m_damage(damage), m_duration(duration)
		{
			m_durationTimer = 0;
		}
	};
}
