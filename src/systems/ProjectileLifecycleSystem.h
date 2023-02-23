#pragma once

#include "../components/ProjectileComponent.h"
#include "../ecs/ECS.h"

class ProjectileLifecycleSystem : public System
{
public:
	ProjectileLifecycleSystem()
	{
		RequireComponent<ProjectileComponent>();
	}

	void Update(const float deltaTime) const
	{
		for (auto entity : GetEntities())
		{
			auto& projectile = entity.GetComponent<ProjectileComponent>();

			projectile.m_durationTimer += deltaTime;
			if(projectile.m_durationTimer >= projectile.m_duration)
				entity.Destroy();
		}
	}
};
