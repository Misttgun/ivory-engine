#pragma once

#include <glm/vec2.hpp>

struct BoxColliderComponent
{
	glm::vec2 m_size;
	glm::vec2 m_offset;
	SDL_Color m_debugColor{};

	BoxColliderComponent() = default;

	explicit BoxColliderComponent(const glm::vec2& size, const glm::vec2& offset = glm::vec2(0)) : m_size(size), m_offset(offset)
	{
		m_debugColor = {255, 255, 0, 255};
	}
};
