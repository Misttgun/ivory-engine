#pragma once

#include <SDL2/SDL_render.h>

#include "../components/BoxColliderComponent.h"
#include "../components/TransformComponent.h"
#include "../ecs/ECS.h"

class CollisionSystem : public System
{
public:
	CollisionSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update() const
	{
		const auto entities = GetEntities();

		for(std::size_t i = 0; i < entities.size(); ++i)
		{
			const auto a = entities.at(i);
			auto& aTransform = a.GetComponent<TransformComponent>();
			auto& aCollider = a.GetComponent<BoxColliderComponent>();

			for(std::size_t j = i + 1; j < entities.size(); j++)
			{
				const auto b = entities.at(j);
				auto& bTransform = b.GetComponent<TransformComponent>();
				auto& bCollider = b.GetComponent<BoxColliderComponent>();

				const bool isColliding = CheckAABBCollision(aTransform.m_position + aCollider.m_offset, aCollider.m_size * aTransform.m_scale,
															bTransform.m_position + bCollider.m_offset, bCollider.m_size * bTransform.m_scale);

				if(isColliding)
				{
					aCollider.m_debugColor = {255, 0, 0, 255};
					bCollider.m_debugColor = {255, 0, 0, 255};
					Logger::Log("Collision detected between entity id: " + std::to_string(a.GetId()) + " and entity id: " + std::to_string(b.GetId()));
				}
				else
				{
					aCollider.m_debugColor = {255, 255, 0, 255};
					bCollider.m_debugColor = {255, 255, 0, 255};
				}
			}
		}
	}

	void Draw(SDL_Renderer* renderer, const bool isDebug = false) const
	{
		if(isDebug == false)
			return;

		for(auto entity : GetEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& collider = entity.GetComponent<BoxColliderComponent>();
			SDL_Rect boundingBox = {
				static_cast<int>(transform.m_position.x + collider.m_offset.x), static_cast<int>(transform.m_position.y + collider.m_offset.y),
				static_cast<int>(collider.m_size.x * transform.m_scale.x), static_cast<int>(collider.m_size.y * transform.m_scale.y)};

			SDL_SetRenderDrawColor(renderer, collider.m_debugColor.r, collider.m_debugColor.g, collider.m_debugColor.b, collider.m_debugColor.a);
			SDL_RenderDrawRect(renderer, &boundingBox);
		}
	}

private:
	static bool CheckAABBCollision(const glm::vec2& aPos, const glm::vec2& aSize, const glm::vec2& bPos, const glm::vec2& bSize)
	{
		return (aPos.x < bPos.x + bSize.x && aPos.x + aSize.x > bPos.x && aPos.y < bPos.y + bSize.y && aPos.y + aSize.y > bPos.y);
	}
};
