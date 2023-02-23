#pragma once

struct HealthComponent
{
	int m_maxHealth;
	int m_currentHealth;

	HealthComponent() = default;

	explicit HealthComponent(const int maxHealth) : m_maxHealth(maxHealth)
	{
		m_currentHealth = m_maxHealth;
	}
};
