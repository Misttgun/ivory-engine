#pragma once

#include <memory>
#include <SDL_render.h>

#include "Ivory/ResourceManager.h"
#include "Ivory/ECS/Registry.h"
#include "Ivory/ECS/System.h"
#include "Ivory/ECS/components/TextLabelComponent.h"

namespace Ivory
{
	class RenderTextSystem : public System
	{
	public:
		RenderTextSystem()
		{
			RequireComponent<TextLabelComponent>();
		}

		void Draw(SDL_Renderer* renderer, const std::unique_ptr<ResourceManager>& assetStore, const SDL_Rect& camera) const
		{
			for (const auto entity : m_entities)
			{
				const auto textLabel = m_registry->GetComponent<TextLabelComponent>(entity);

				SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont(textLabel.m_assetId), textLabel.m_text.c_str(), textLabel.m_color);
				SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
				SDL_FreeSurface(surface);

				int32 labelWidth = 0;
				int32 labelHeight = 0;

				SDL_QueryTexture(texture, nullptr, nullptr, &labelWidth, &labelHeight);

				SDL_Rect dstRect = {
					static_cast<int32>(textLabel.m_position.x) - (textLabel.m_isFixed ? 0 : camera.x),
					static_cast<int32>(textLabel.m_position.y) - (textLabel.m_isFixed ? 0 : camera.y),
					labelWidth,
					labelHeight
				};

				SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
				SDL_DestroyTexture(texture);
			}
		}
	};
}
