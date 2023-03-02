#pragma once

#include "../components/BoxColliderComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/TextLabelComponent.h"
#include "../components/tags/ProjectileTag.h"
#include "../ecs/ECS.h"
#include "../events/CollisionEvent.h"
#include "../events/EventBus.h"

class EventBus;

class DamageSystem : public System
{
public:
	DamageSystem()
	{
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvent(const std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
	}

private:
	void OnCollision(const CollisionEvent& event)
	{
		const Entity a = event.m_entityA;
		const Entity b = event.m_entityB;

		if(a.HasComponent<ProjectileTag>() && b.HasComponent<PlayerTag>())
		{
			OnProjectileHitsEntity(a, b, false);
		}
		else if(a.HasComponent<PlayerTag>() && b.HasComponent<ProjectileTag>())
		{
			OnProjectileHitsEntity(b, a, false);
		}
		else if(a.HasComponent<ProjectileTag>() && b.HasComponent<EnemyTag>())
		{
			OnProjectileHitsEntity(a, b, true);
		}
		else if(a.HasComponent<EnemyTag>() && b.HasComponent<ProjectileTag>())
		{
			OnProjectileHitsEntity(b, a, true);
		}

		//Logger::Log("Collision detected between entity id: " + std::to_string(a.GetId()) + " and entity id: " + std::to_string(b.GetId()));
	}

	static void OnProjectileHitsEntity(const Entity projectile, const Entity entity, const bool isProjectileFriendly)
	{
		const auto& projectileComp = projectile.GetComponent<ProjectileComponent>();
		if(projectileComp.m_isFriendly != isProjectileFriendly)
			return;

		auto& heathComp = entity.GetComponent<HealthComponent>();
		heathComp.m_currentHealth -= projectileComp.m_damage;

		if(heathComp.m_currentHealth <= 0)
			entity.Destroy();

		projectile.Destroy();
	}
};
