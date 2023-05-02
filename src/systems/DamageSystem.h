#pragma once

#include "../components/BoxColliderComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/tags/ProjectileTag.h"
#include "../core/Registry.h"
#include "../events/CollisionEvent.h"
#include "../events/EventBus.h"

namespace re
{
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

			if (m_registry->HasComponent<ProjectileTag>(a) && m_registry->HasComponent<PlayerTag>(b))
			{
				OnProjectileHitsEntity(a, b, false);
			}
			else if (m_registry->HasComponent<PlayerTag>(a) && m_registry->HasComponent<ProjectileTag>(b))
			{
				OnProjectileHitsEntity(b, a, false);
			}
			else if (m_registry->HasComponent<ProjectileTag>(a) && m_registry->HasComponent<EnemyTag>(b))
			{
				OnProjectileHitsEntity(a, b, true);
			}
			else if (m_registry->HasComponent<EnemyTag>(a) && m_registry->HasComponent<ProjectileTag>(b))
			{
				OnProjectileHitsEntity(b, a, true);
			}
		}

		void OnProjectileHitsEntity(const Entity projectile, const Entity entity, const bool isProjectileFriendly) const
		{
			const auto& projectileComp = m_registry->GetComponent<ProjectileComponent>(projectile);
			if (projectileComp.m_isFriendly != isProjectileFriendly)
				return;

			auto& heathComp = m_registry->GetComponent<HealthComponent>(entity);
			heathComp.m_currentHealth -= projectileComp.m_damage;

			if (heathComp.m_currentHealth <= 0)
				m_registry->DestroyEntity(entity);

			m_registry->DestroyEntity(projectile);
		}
	};
}
