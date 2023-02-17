#pragma once

#include "../components/RigidBodyComponent.h"
#include "../components/TransformComponent.h"
#include "../ecs/ECS.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void Update(const float deltaTime) const
	{

		for(auto entity : GetEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			transform.m_position += rigidBody.m_velocity * deltaTime;
		}
	}
};
