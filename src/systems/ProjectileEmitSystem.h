#pragma once

#include "../components/ProjectileComponent.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/TransformComponent.h"
#include "../components/tags/ProjectileTag.h"
#include "../core/System.h"

namespace re
{
	extern Registry registry;

	class ProjectileEmitSystem : public System
	{
	public:
		ProjectileEmitSystem()
		{
			RequireComponent<ProjectileEmitterComponent>();
			RequireComponent<TransformComponent>();
		}

		void Update(const float deltaTime) const
		{
			for (const auto entity : GetEntities())
			{
				//Ignore the player
				if (registry.HasComponent<PlayerTag>(entity))
					continue;

				auto& projectileEmitter = registry.GetComponent<ProjectileEmitterComponent>(entity);
				const auto& transform = registry.GetComponent<TransformComponent>(entity);

				projectileEmitter.m_cooldownTimer += deltaTime;

				if (projectileEmitter.m_cooldownTimer >= projectileEmitter.m_cooldown)
				{
					glm::vec2 projectilePosition = transform.m_position;
					if (registry.HasComponent<SpriteComponent>(entity))
					{
						const auto sprite = registry.GetComponent<SpriteComponent>(entity);
						projectilePosition.x += (transform.m_scale.x * static_cast<float>(sprite.m_width) / 2.0f);
						projectilePosition.y += (transform.m_scale.y * static_cast<float>(sprite.m_height) / 2.0f);
					}

					// Add a new projectile entity to the registry
					const Entity projectile = registry.CreateEntity();
					registry.AddComponent(projectile, ProjectileTag{});
					registry.AddComponent(projectile, TransformComponent{ projectilePosition });
					registry.AddComponent(projectile, RigidBodyComponent{ projectileEmitter.m_projectileVelocity });
					registry.AddComponent(projectile, SpriteComponent{ "bullet-image", 4, 4, 1 });
					registry.AddComponent(projectile, BoxColliderComponent{ glm::vec2(4, 4) });
					registry.AddComponent(projectile, ProjectileComponent{ projectileEmitter.m_isFriendly, projectileEmitter.m_projectileDamage, projectileEmitter.m_projectileDuration });

					projectileEmitter.m_cooldownTimer = 0;
				}
			}
		}

		void SubscribeToEvent(const std::shared_ptr<EventBus>& eventBus)
		{
			eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
		}

	private:
		void OnKeyPressed(const KeyPressedEvent& event)
		{
			if (event.m_symbol != SDLK_SPACE)
				return;

			for (const auto entity : GetEntities())
			{
				if (registry.HasComponent<PlayerTag>(entity) == false)
					continue;

				auto& projectileEmitter = registry.GetComponent<ProjectileEmitterComponent>(entity);
				const auto& transform = registry.GetComponent<TransformComponent>(entity);
				const auto& rigidBody = registry.GetComponent<RigidBodyComponent>(entity);

				glm::vec2 projectilePosition = transform.m_position;
				if (registry.HasComponent<SpriteComponent>(entity))
				{
					const auto sprite = registry.GetComponent<SpriteComponent>(entity);
					projectilePosition.x += (transform.m_scale.x * static_cast<float>(sprite.m_width) / 2.0f);
					projectilePosition.y += (transform.m_scale.y * static_cast<float>(sprite.m_height) / 2.0f);
				}

				// Because we only have four direction, normalizing the velocity is going to give us the direction horizontal or vertical
				glm::vec2 normalizedVelocity = glm::normalize(rigidBody.m_velocity);

				// Add a new projectile entity to the registry
				const Entity projectile = registry.CreateEntity();
				registry.AddComponent(projectile, ProjectileTag{});
				registry.AddComponent(projectile, TransformComponent{ projectilePosition });
				registry.AddComponent(projectile, RigidBodyComponent{ projectileEmitter.m_projectileVelocity * normalizedVelocity });
				registry.AddComponent(projectile, SpriteComponent{ "bullet-image", 4, 4, 4 });
				registry.AddComponent(projectile, BoxColliderComponent{ glm::vec2(4, 4) });
				registry.AddComponent(projectile, ProjectileComponent{ projectileEmitter.m_isFriendly, projectileEmitter.m_projectileDamage, projectileEmitter.m_projectileDuration });

				projectileEmitter.m_cooldownTimer = 0;
			}
		}
	};
}
