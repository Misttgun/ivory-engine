#pragma once

namespace Ivory
{
	struct ProjectileEmitterComponent
	{
		glm::vec2 m_projectileVelocity;
		float m_cooldown;
		float m_projectileDuration;
		int32 m_projectileDamage;
		bool m_isFriendly;
		float m_cooldownTimer;

		ProjectileEmitterComponent() = default;

		explicit ProjectileEmitterComponent(const glm::vec2 velocity, const float cooldown, const float duration = 5, const int32 damage = 10, const bool isFriendly = false) :
			m_projectileVelocity(velocity), m_cooldown(cooldown), m_projectileDuration(duration), m_projectileDamage(damage), m_isFriendly(isFriendly)
		{
			m_cooldownTimer = 0.0f;
		}
	};
}
