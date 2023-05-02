#pragma once

#include "../components/BoxColliderComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/tags/ProjectileTag.h"
#include "../core/System.h"
#include "../events/CollisionEvent.h"
#include "../events/EventBus.h"

namespace re
{
	extern Registry registry;

	class DamageSystem : public System
	{
	public:
		DamageSystem()
		{
			RequireComponent<BoxColliderComponent>();
		}

		void SubscribeToEvent(const std::shared_ptr<EventBus>& eventBus)
		{
			eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
		}

	private:
		void OnCollision(const CollisionEvent& event)
		{
			const Entity a = event.m_entityA;
			const Entity b = event.m_entityB;

			if (registry.HasComponent<ProjectileTag>(a) && registry.HasComponent<PlayerTag>(b))
			{
				OnProjectileHitsEntity(a, b, false);
			}
			else if (registry.HasComponent<PlayerTag>(a) && registry.HasComponent<ProjectileTag>(b))
			{
				OnProjectileHitsEntity(b, a, false);
			}
			else if (registry.HasComponent<ProjectileTag>(a) && registry.HasComponent<EnemyTag>(b))
			{
				OnProjectileHitsEntity(a, b, true);
			}
			else if (registry.HasComponent<EnemyTag>(a) && registry.HasComponent<ProjectileTag>(b))
			{
				OnProjectileHitsEntity(b, a, true);
			}
		}

		static void OnProjectileHitsEntity(const Entity projectile, const Entity entity, const bool isProjectileFriendly)
		{
			const auto& projectileComp = registry.GetComponent<ProjectileComponent>(projectile);
			if (projectileComp.m_isFriendly != isProjectileFriendly)
				return;

			auto& heathComp = registry.GetComponent<HealthComponent>(entity);
			heathComp.m_currentHealth -= projectileComp.m_damage;

			if (heathComp.m_currentHealth <= 0)
				registry.DestroyEntity(entity);

			registry.DestroyEntity(projectile);
		}
	};
}
