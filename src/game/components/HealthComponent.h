#pragma once

namespace re
{
	struct HealthComponent
	{
		int32 m_maxHealth;
		int32 m_currentHealth;

		HealthComponent() = default;

		explicit HealthComponent(const int32 maxHealth) : m_maxHealth(maxHealth)
		{
			m_currentHealth = m_maxHealth;
		}
	};
}
