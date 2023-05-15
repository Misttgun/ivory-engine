#pragma once


namespace Ivory
{
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
			for (const auto entity : m_entities)
			{
				auto& transform = m_registry->GetComponent<TransformComponent>(entity);
				const auto& rigidBody = m_registry->GetComponent<RigidBodyComponent>(entity);

				transform.m_position += rigidBody.m_velocity * deltaTime;

				const bool isEntityOutsideMap = transform.m_position.x < 0 || transform.m_position.x > static_cast<float>(Level::m_mapWidth) ||
					transform.m_position.y < 0 || transform.m_position.y > static_cast<float>(Level::m_mapHeight);

				if (isEntityOutsideMap)
				{
					// Kill all enemies that get outside of the map boundaries
					if (m_registry->HasComponent<PlayerTag>(entity) == false)
					{
						m_registry->DestroyEntity(entity);
					}
					else
					{
						auto position = transform.m_position;
						position.x = glm::clamp(position.x, 0.0f, static_cast<float>(Level::m_mapWidth));
						position.y = glm::clamp(position.y, 0.0f, static_cast<float>(Level::m_mapHeight));
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

			if (m_registry->HasComponent<EnemyTag>(a) && m_registry->HasComponent<ObstacleTag>(b))
			{
				OnEnemyHitsObstacle(a);
			}
			else if (m_registry->HasComponent<ObstacleTag>(a) && m_registry->HasComponent<EnemyTag>(b))
			{
				OnEnemyHitsObstacle(b);
			}
		}

		void OnEnemyHitsObstacle(const Entity enemy) const
		{
			if (m_registry->HasComponent<RigidBodyComponent>(enemy) == false || m_registry->HasComponent<SpriteComponent>(enemy) == false)
				return;

			auto& rigidBody = m_registry->GetComponent<RigidBodyComponent>(enemy);
			auto& sprite = m_registry->GetComponent<SpriteComponent>(enemy);

			if (glm::abs(rigidBody.m_velocity.x - 0) > 0.001f)
			{
				rigidBody.m_velocity.x *= -1;
				sprite.m_flip = sprite.m_flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}

			if (glm::abs(rigidBody.m_velocity.y - 0) > 0.001f)
			{
				rigidBody.m_velocity.y *= -1;
				sprite.m_flip = sprite.m_flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			}
		}
	};
}
