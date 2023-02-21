#pragma once

#include <algorithm>
#include "../helpers/AssetStore.h"
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

	void Draw(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& assetStore) const
	{
		// Sorting entities by Z Index. Higher Z index get drawn on top of lower Z Index.
		auto entities = GetEntities();

		std::ranges::sort(entities, [](const Entity& a, const Entity& b)
		{
			const auto& spriteA = a.GetComponent<SpriteComponent>();
			const auto& spriteB = b.GetComponent<SpriteComponent>();
			return spriteA.m_zIndex < spriteB.m_zIndex;
		});


		for (auto entity : entities)
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect srcRect = sprite.m_srcRect;

			SDL_Rect dstRect = { static_cast<int>(transform.m_position.x), static_cast<int>(transform.m_position.y),
			sprite.m_width * static_cast<int>(transform.m_scale.x), sprite.m_height * static_cast<int>(transform.m_scale.y) };


			auto texture = assetStore->GetTexture(sprite.m_assetId);
			SDL_RenderCopyEx(renderer, assetStore->GetTexture(sprite.m_assetId), &srcRect, &dstRect, transform.m_rotation, nullptr, SDL_FLIP_NONE);
		}
	}
};
