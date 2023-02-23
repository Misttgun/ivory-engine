#pragma once
#include <glm/vec2.hpp>

struct KeyboardControlComponent
{
	glm::vec2 m_up;
	glm::vec2 m_right;
	glm::vec2 m_down;
	glm::vec2 m_left;

	KeyboardControlComponent() = default;

	KeyboardControlComponent(const glm::vec2& up, const glm::vec2& right, const glm::vec2& down, const glm::vec2& left)
		: m_up(up), m_right(right), m_down(down), m_left(left)
	{}
};
