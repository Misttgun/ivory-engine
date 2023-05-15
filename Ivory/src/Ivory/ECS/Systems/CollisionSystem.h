#pragma once

#include <memory>
#include <SDL_render.h>

#include "Ivory/ECS/Registry.h"
#include "Ivory/ECS/System.h"
#include "Ivory/ECS/components/BoxColliderComponent.h"
#include "Ivory/ECS/components/TransformComponent.h"
#include "Ivory/Events/CollisionEvent.h"
#include "Ivory/Events/DestroyEvent.h"
#include "Ivory/Events/EventBus.h"

namespace Ivory
{
	class CollisionSystem : public System
	{
	public:
		CollisionSystem()
		{
			RequireComponent<TransformComponent>();
			RequireComponent<BoxColliderComponent>();
		}

		void SubscribeToEvent(const std::shared_ptr<EventBus>& eventBus)
		{
			eventBus->SubscribeToEvent<DestroyEvent>(this, &CollisionSystem::OnEntityDestroyed);
		}

		void Update(const std::shared_ptr<EventBus>& eventBus) const
		{
			const auto entities = m_entities;

			for (auto entityA = entities.begin(); entityA != entities.end(); ++entityA)
			{
				const auto a = *entityA;
				auto& aTransform = m_registry->GetComponent<TransformComponent>(a);
				auto& aCollider = m_registry->GetComponent<BoxColliderComponent>(a);

				auto entityB = entityA;
				std::advance(entityB, 1); // Set entityB iterator to 
				for (; entityB != entities.end(); ++entityB)
				{
					const auto b = *entityB;
					auto& bTransform = m_registry->GetComponent<TransformComponent>(b);
					auto& bCollider = m_registry->GetComponent<BoxColliderComponent>(b);

					const bool isColliding = CheckAABBCollision(aTransform.m_position + aCollider.m_offset, aCollider.m_size * aTransform.m_scale,
					                                            bTransform.m_position + bCollider.m_offset, bCollider.m_size * bTransform.m_scale);

					if (isColliding)
					{
						if (aCollider.IsCollidingWithEntity(b) == true) // If is colliding with b, b is colliding with a
							continue;

						aCollider.AddCollidingEntity(b);
						bCollider.AddCollidingEntity(a);
						eventBus->EmitEvent<CollisionEvent>(a, b);
					}
					else
					{
						aCollider.RemoveCollidingEntity(b);
						bCollider.RemoveCollidingEntity(a);
					}
				}

				if (aCollider.IsCollidingWithAnyEntity())
					aCollider.m_debugColor = {255, 0, 0, 255};
				else
					aCollider.m_debugColor = {255, 255, 0, 255};
			}
		}

		void Draw(SDL_Renderer* renderer, const SDL_Rect& camera) const
		{
			for (const auto entity : m_entities)
			{
				const auto& transform = m_registry->GetComponent<TransformComponent>(entity);
				const auto& collider = m_registry->GetComponent<BoxColliderComponent>(entity);
				SDL_Rect boundingBox = {
					static_cast<int>(transform.m_position.x + collider.m_offset.x - camera.x),
					static_cast<int>(transform.m_position.y + collider.m_offset.y - camera.y),
					static_cast<int>(collider.m_size.x * transform.m_scale.x),
					static_cast<int>(collider.m_size.y * transform.m_scale.y)
				};

				SDL_SetRenderDrawColor(renderer, collider.m_debugColor.r, collider.m_debugColor.g, collider.m_debugColor.b, collider.m_debugColor.a);
				SDL_RenderDrawRect(renderer, &boundingBox);
			}
		}

	private:
		static bool CheckAABBCollision(const glm::vec2& aPos, const glm::vec2& aSize, const glm::vec2& bPos, const glm::vec2& bSize)
		{
			return (aPos.x < bPos.x + bSize.x && aPos.x + aSize.x > bPos.x && aPos.y < bPos.y + bSize.y && aPos.y + aSize.y > bPos.y);
		}

		void OnEntityDestroyed(const DestroyEvent& event)
		{
			const auto destroyedEntity = event.m_destroyedEntity;

			for (const auto entity : m_entities)
			{
				auto& collider = m_registry->GetComponent<BoxColliderComponent>(entity);
				collider.RemoveCollidingEntity(destroyedEntity);
			}
		}
	};
}
