#pragma once

struct SpriteComponent
{
	std::string m_assetId;
	int m_width;
	int m_height;
	int m_zIndex;
	SDL_Rect m_srcRect = {};

	SpriteComponent() = default;

	explicit SpriteComponent(std::string assetId, const int width, const int height, const int zIndex = 0, const int rectX = 0, const int rectY = 0)
		:m_assetId(std::move(assetId)), m_width(width), m_height(height), m_zIndex(zIndex)
	{
		m_srcRect = {rectX, rectY, width, height};
	}
};
