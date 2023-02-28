#pragma once

#include <SDL2/SDL.h>
#include "../Components/TextLabelComponent.h"
#include "../ECS/ECS.h"
#include "../helpers/AssetStore.h"

class RenderTextSystem : public System
{
public:
	RenderTextSystem()
	{
		RequireComponent<TextLabelComponent>();
	}

	void Draw(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) const
	{
		for(auto entity : GetEntities())
		{
			const auto textLabel = entity.GetComponent<TextLabelComponent>();

			SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont(textLabel.m_assetId), textLabel.m_text.c_str(), textLabel.m_color);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);

			int labelWidth = 0;
			int labelHeight = 0;

			SDL_QueryTexture(texture, nullptr, nullptr, &labelWidth, &labelHeight);

			SDL_Rect dstRect = {
				static_cast<int>(textLabel.m_position.x) - (textLabel.m_isFixed ? 0 : camera.x),
				static_cast<int>(textLabel.m_position.y) - (textLabel.m_isFixed ? 0 : camera.y),
				labelWidth,
				labelHeight
			};

			SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
			SDL_DestroyTexture(texture);
		}
	}
};
