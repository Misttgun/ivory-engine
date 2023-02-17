#pragma once

#include "../components/SpriteComponent.h"
#include "../ecs/ECS.h"

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer) const
	{
		for (auto entity : GetEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect rect = {static_cast<int>(transform.m_position.x), static_cast<int>(transform.m_position.y), sprite.m_width, sprite.m_height};
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(renderer, &rect);
		}
	}
};
