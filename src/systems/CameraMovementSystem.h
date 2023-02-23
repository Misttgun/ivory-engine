#pragma once
#include "../components/CameraFollowComponent.h"
#include "../ecs/ECS.h"


struct RigidBodyComponent;

class CameraMovementSystem : public System
{
public:
	CameraMovementSystem()
	{
		RequireComponent<CameraFollowComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(SDL_Rect& camera) const
	{
		for(auto entity : GetEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();

			camera.x = glm::clamp(static_cast<int>(transform.m_position.x - camera.w / 2.0f), 0, Game::m_mapWidth - camera.w);
			camera.y = glm::clamp(static_cast<int>(transform.m_position.y - camera.h / 2.0f), 0, Game::m_mapHeight - camera.h);
		}
	}
};
