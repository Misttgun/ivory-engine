#pragma once

#include <glm/glm.hpp>

namespace re
{
	struct TransformComponent
	{
		glm::vec2 m_position;
		glm::vec2 m_scale;
		double m_rotation{};

		TransformComponent() = default;

		explicit TransformComponent(const glm::vec2 position, const glm::vec2 scale = glm::vec2(1), const double rotation = 0) : m_position(position), m_scale(scale), m_rotation(rotation) { }
	};
}
