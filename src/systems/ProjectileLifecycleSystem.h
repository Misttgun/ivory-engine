#pragma once

#include "../components/ProjectileComponent.h"
#include "../core/System.h"

namespace re
{
	extern Registry registry;

	class ProjectileLifecycleSystem : public System
	{
	public:
		ProjectileLifecycleSystem()
		{
			RequireComponent<ProjectileComponent>();
		}

		void Update(const float deltaTime) const
		{
			for (const auto entity : GetEntities())
			{
				auto& projectile = registry.GetComponent<ProjectileComponent>(entity);

				projectile.m_durationTimer += deltaTime;
				if (projectile.m_durationTimer >= projectile.m_duration)
					registry.DestroyEntity(entity);
			}
		}
	};
}
