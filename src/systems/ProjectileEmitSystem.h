#pragma once

#include "../components/ProjectileComponent.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/TransformComponent.h"
#include "../ecs/ECS.h"

class ProjectileEmitSystem : public System
{
public:
	ProjectileEmitSystem()
	{
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(const float deltaTime, const std::unique_ptr<Registry>& registry) const
	{
		for(auto entity : GetEntities())
		{
			//Ignore the player
			if(entity.HasComponent<KeyboardControlComponent>())
				continue;

			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			const auto& transform = entity.GetComponent<TransformComponent>();

			projectileEmitter.m_cooldownTimer += deltaTime;

			if(projectileEmitter.m_cooldownTimer >= projectileEmitter.m_cooldown)
			{
				glm::vec2 projectilePosition = transform.m_position;
				if(entity.HasComponent<SpriteComponent>())
				{
					const auto sprite = entity.GetComponent<SpriteComponent>();
					projectilePosition.x += (transform.m_scale.x * static_cast<float>(sprite.m_width) / 2.0f);
					projectilePosition.y += (transform.m_scale.y * static_cast<float>(sprite.m_height) / 2.0f);
				}

				// Add a new projectile entity to the registry
				Entity projectile = registry->CreateEntity();
				projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
				projectile.AddComponent<RigidBodyComponent>(projectileEmitter.m_projectileVelocity);
				projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
				projectile.AddComponent<BoxColliderComponent>(glm::vec2(4, 4));
				projectile.AddComponent<ProjectileComponent>(projectileEmitter.m_isFriendly, projectileEmitter.m_projectileDamage, projectileEmitter.m_projectileDuration);

				projectileEmitter.m_cooldownTimer = 0;
			}
		}
	}

	void SubscribeToEvent(const std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
	}

private:
	void OnKeyPressed(const KeyPressedEvent& event)
	{
		if(event.m_symbol != SDLK_SPACE)
			return;

		for(auto entity : GetEntities())
		{
			if(entity.HasComponent<KeyboardControlComponent>() == false)
				continue;

			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			glm::vec2 projectilePosition = transform.m_position;
			if(entity.HasComponent<SpriteComponent>())
			{
				const auto sprite = entity.GetComponent<SpriteComponent>();
				projectilePosition.x += (transform.m_scale.x * static_cast<float>(sprite.m_width) / 2.0f);
				projectilePosition.y += (transform.m_scale.y * static_cast<float>(sprite.m_height) / 2.0f);
			}

			// Because we only have four direction, normalizing the velocity is going to give us the direction horizontal or vertical
			glm::vec2 normalizedVelocity = glm::normalize(rigidBody.m_velocity);

			// Add a new projectile entity to the registry
			Entity projectile = entity.GetRegistry()->CreateEntity();
			projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
			projectile.AddComponent<RigidBodyComponent>(projectileEmitter.m_projectileVelocity * normalizedVelocity);
			projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
			projectile.AddComponent<BoxColliderComponent>(glm::vec2(4, 4));
			projectile.AddComponent<ProjectileComponent>(projectileEmitter.m_isFriendly, projectileEmitter.m_projectileDamage, projectileEmitter.m_projectileDuration);

			projectileEmitter.m_cooldownTimer = 0;
		}
	}
};
