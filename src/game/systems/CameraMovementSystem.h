#pragma once

#include "../components/tags/CameraFollowTag.h"
#include "../engine/core/ecs/Registry.h"

namespace re
{
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
			for (const auto entity : m_entities)
			{
				const auto& transform = m_registry->GetComponent<TransformComponent>(entity);

				camera.x = glm::clamp(static_cast<int>(transform.m_position.x - camera.w / 2.0f), 0, utils::Level::m_mapWidth - camera.w);
				camera.y = glm::clamp(static_cast<int>(transform.m_position.y - camera.h / 2.0f), 0, utils::Level::m_mapHeight - camera.h);
			}
		}
	};
}
