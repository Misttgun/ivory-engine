#pragma once

#include <algorithm>

#include "../components/SpriteComponent.h"
#include "../core/ResourceManager.h"
#include "../core/ecs/System.h"

namespace re
{
	class RenderSystem : public System
	{
	public:
		RenderSystem()
		{
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
		}

		void Draw(SDL_Renderer* renderer, const std::unique_ptr<ResourceManager>& assetStore, const SDL_Rect& camera) const
		{
			struct RenderableEntity
			{
				TransformComponent m_transform{};
				SpriteComponent m_sprite{};
			};

			// Sorting entities by Z Index. Higher Z index get drawn on top of lower Z Index.
			const auto entities = m_entities;
			std::vector<RenderableEntity> visibleEntities;

			for (auto entity : entities)
			{
				RenderableEntity rEntity;
				rEntity.m_transform = m_registry->GetComponent<TransformComponent>(entity);
				rEntity.m_sprite = m_registry->GetComponent<SpriteComponent>(entity);

				const auto position = rEntity.m_transform.m_position;
				const auto scale = rEntity.m_transform.m_scale;
				const auto sprite = rEntity.m_sprite;

				bool isOutsideCameraView = position.x + (scale.x * static_cast<float>(sprite.m_width)) < static_cast<float>(camera.x) || position.x > static_cast<float>(camera.x + camera.w)
					|| position.y + (scale.y * static_cast<float>(sprite.m_height)) < static_cast<float>(camera.y) || position.y > static_cast<float>(camera.y + camera.h);

				if (isOutsideCameraView && sprite.m_isFixed == false)
					continue;

				visibleEntities.push_back(rEntity);
			}

			std::ranges::sort(visibleEntities, [](const RenderableEntity& a, const RenderableEntity& b)
			{
				return a.m_sprite.m_zIndex < b.m_sprite.m_zIndex;
			});

			for (const auto& [transform, sprite] : visibleEntities)
			{
				SDL_Rect srcRect = sprite.m_srcRect;

				SDL_Rect dstRect = {
					static_cast<int32>(transform.m_position.x - (sprite.m_isFixed ? 0 : static_cast<float>(camera.x))),
					static_cast<int32>(transform.m_position.y - (sprite.m_isFixed ? 0 : static_cast<float>(camera.y))), sprite.m_width * static_cast<int>(transform.m_scale.x),
					sprite.m_height * static_cast<int32>(transform.m_scale.y)
				};

				auto texture = assetStore->GetTexture(sprite.m_assetId);
				SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, transform.m_rotation, nullptr, sprite.m_flip);
			}
		}
	};
}
