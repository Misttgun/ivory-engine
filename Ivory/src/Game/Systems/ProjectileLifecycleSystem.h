#pragma once

namespace Ivory
{
	class ProjectileLifecycleSystem : public System
	{
	public:
		ProjectileLifecycleSystem()
		{
			RequireComponent<ProjectileComponent>();
		}

		void Update(const float deltaTime) const
		{
			for (const auto entity : m_entities)
			{
				auto& projectile = m_registry->GetComponent<ProjectileComponent>(entity);

				projectile.m_durationTimer += deltaTime;
				if (projectile.m_durationTimer >= projectile.m_duration)
					m_registry->DestroyEntity(entity);
			}
		}
	};
}
