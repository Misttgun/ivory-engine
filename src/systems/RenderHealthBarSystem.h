#pragma once

#include <SDL2/SDL.h>

#include "../components/HealthComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/TransformComponent.h"
#include "../core/ResourceManager.h"
#include "../core/System.h"

namespace re
{
	class RenderHealthBarSystem : public System
	{
	public:
		RenderHealthBarSystem()
		{
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
			RequireComponent<HealthComponent>();
		}

		void Draw(SDL_Renderer* renderer, const std::unique_ptr<ResourceManager>& assetStore, const SDL_Rect& camera) const
		{
			for (const auto entity : m_entities)
			{
				const auto transform = m_registry->GetComponent<TransformComponent>(entity);
				const auto sprite = m_registry->GetComponent<SpriteComponent>(entity);
				const auto health = m_registry->GetComponent<HealthComponent>(entity);

				// Draw a the health bar with the correct color for the percentage
				SDL_Color healthBarColor = {255, 255, 255, 255};
				const auto healthPercentage = static_cast<int32>((static_cast<float>(health.m_currentHealth) / static_cast<float>(health.m_maxHealth)) * 100);

				if (healthPercentage >= 0 && healthPercentage < 40)
				{
					// 0-40 = red
					healthBarColor = {255, 0, 0, 255};
				}
				if (healthPercentage >= 40 && healthPercentage < 80)
				{
					// 40-80 = yellow
					healthBarColor = {255, 255, 0, 255};
				}
				if (healthPercentage >= 80 && healthPercentage <= 100)
				{
					// 80-100 = green
					healthBarColor = {0, 255, 0, 255};
				}

				// Position the health bar indicator in the top-right part of the entity sprite
				constexpr int32 healthBarWidth = 15;
				constexpr int32 healthBarHeight = 3;
				const int32 healthBarPosX = static_cast<int32>(transform.m_position.x + static_cast<float>(sprite.m_width) * transform.m_scale.x) - camera.x;
				const int32 healthBarPosY = static_cast<int32>(transform.m_position.y) - camera.y;

				SDL_Rect healthBarRectangle = {
					static_cast<int32>(healthBarPosX),
					static_cast<int32>(healthBarPosY),
					static_cast<int32>(healthBarWidth * (healthPercentage / 100.0f)),
					static_cast<int32>(healthBarHeight)
				};
				SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
				SDL_RenderFillRect(renderer, &healthBarRectangle);

				// Render the health percentage text label indicator
				std::string healthText = std::to_string(healthPercentage);
				SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont("pico8-font-5"), healthText.c_str(), healthBarColor);
				SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
				SDL_FreeSurface(surface);

				int32 labelWidth = 0;
				int32 labelHeight = 0;
				SDL_QueryTexture(texture, nullptr, nullptr, &labelWidth, &labelHeight);
				SDL_Rect healthBarTextRectangle = {static_cast<int32>(healthBarPosX), static_cast<int32>(healthBarPosY) + 5, labelWidth, labelHeight};

				SDL_RenderCopy(renderer, texture, nullptr, &healthBarTextRectangle);

				SDL_DestroyTexture(texture);
			}
		}
	};
}
