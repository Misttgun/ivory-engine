#pragma once

#include "../components/tags/CameraFollowTag.h"
#include "../core/System.h"

namespace re
{
	extern Registry registry;

	struct RigidBodyComponent;

	class CameraMovementSystem : public System
	{
	public:
		CameraMovementSystem()
		{
			RequireComponent<CameraFollowTag>();
			RequireComponent<TransformComponent>();
		}

		void Update(SDL_Rect& camera) const
		{
			for (const auto entity : GetEntities())
			{
				const auto& transform = registry.GetComponent<TransformComponent>(entity);

				camera.x = glm::clamp(static_cast<int>(transform.m_position.x - camera.w / 2.0f), 0, Game::m_mapWidth - camera.w);
				camera.y = glm::clamp(static_cast<int>(transform.m_position.y - camera.h / 2.0f), 0, Game::m_mapHeight - camera.h);
			}
		}
	};
}
