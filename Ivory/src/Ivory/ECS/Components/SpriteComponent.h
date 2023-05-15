#pragma once

#include <SDL_render.h>

namespace Ivory
{
	struct SpriteComponent
	{
		std::string m_assetId;
		int32 m_width{};
		int32 m_height{};
		int32 m_zIndex{};
		bool m_isFixed{};
		SDL_RendererFlip m_flip = SDL_FLIP_NONE;
		SDL_Rect m_srcRect = {};

		SpriteComponent() = default;

		explicit SpriteComponent(std::string assetId, const int32 width, const int32 height, const int32 zIndex = 0, const bool isFixed = false, const int rectX = 0,
		                         const int rectY = 0) : m_assetId(std::move(assetId)), m_width(width), m_height(height), m_zIndex(zIndex), m_isFixed(isFixed)
		{
			m_srcRect = {rectX, rectY, width, height};
		}
	};
}
