#pragma once

#include <SDL_pixels.h>
#include <string>

#include <glm/glm.hpp>

namespace Ivory
{
	struct TextLabelComponent
	{
		glm::vec2 m_position{};
		std::string m_text;
		std::string m_assetId;
		SDL_Color m_color{};
		bool m_isFixed{};

		TextLabelComponent() = default;

		explicit TextLabelComponent(const glm::vec2 position, std::string text, std::string assetId, const SDL_Color& color, const bool isFixed = true)
			: m_position(position), m_text(std::move(text)), m_assetId(std::move(assetId)), m_color(color), m_isFixed(isFixed) { }
	};
}
