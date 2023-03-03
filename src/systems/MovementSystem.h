#pragma once

#include "../components/RigidBodyComponent.h"
#include "../components/TransformComponent.h"
#include "../components/tags/ObstacleTag.h"
#include "../ecs/ECS.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void Update(const float deltaTime) const
	{

		for(auto entity : GetEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			transform.m_position += rigidBody.m_velocity * deltaTime;

			const bool isEntityOutsideMap = transform.m_position.x < 0 || transform.m_position.x > static_cast<float>(Game::m_mapWidth)
				|| transform.m_position.y < 0 || transform.m_position.y > static_cast<float>(Game::m_mapHeight);

			if(isEntityOutsideMap)
			{
				// Kill all enemies that get outside of the map boundaries
				if(entity.HasComponent<PlayerTag>() == false)
				{
					entity.Destroy();
				}
				else
				{
					auto position = transform.m_position;
					position.x = glm::clamp(position.x, 0.0f, static_cast<float>(Game::m_mapWidth));
					position.y = glm::clamp(position.y, 0.0f, static_cast<float>(Game::m_mapHeight));
					transform.m_position = position;
				}
			}
		}
	}

	void SubscribeToEvent(const std::shared_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
	}

private:
	void OnCollision(const CollisionEvent& event)
	{
		const Entity a = event.m_entityA;
		const Entity b = event.m_entityB;

		if(a.HasComponent<EnemyTag>() && b.HasComponent<ObstacleTag>())
		{
			OnEnemyHitsObstacle(a);
		}
		else if(a.HasComponent<ObstacleTag>() && b.HasComponent<EnemyTag>())
		{
			OnEnemyHitsObstacle(b);
		}
	}

	static void OnEnemyHitsObstacle(const Entity enemy)
	{
		if(enemy.HasComponent<RigidBodyComponent>() == false || enemy.HasComponent<SpriteComponent>() == false)
			return;

		auto& rigidBody = enemy.GetComponent<RigidBodyComponent>();
		auto& sprite = enemy.GetComponent<SpriteComponent>();

		if(glm::abs(rigidBody.m_velocity.x - 0) > 0.001f)
		{
			rigidBody.m_velocity.x *= -1;
			sprite.m_flip = sprite.m_flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		}

		if(glm::abs(rigidBody.m_velocity.y - 0) > 0.001f)
		{
			rigidBody.m_velocity.y *= -1;
			sprite.m_flip = sprite.m_flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
		}
	}
};
