#pragma once

namespace Ivory
{
	struct RigidBodyComponent
	{
		glm::vec2 m_velocity;

		RigidBodyComponent() = default;

		explicit RigidBodyComponent(const glm::vec2 velocity) : m_velocity(velocity) { }
	};
}
