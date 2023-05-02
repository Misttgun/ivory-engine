#pragma once

#include "../components/AnimationComponent.h"
#include "../components/SpriteComponent.h"
#include "../core/System.h"

namespace re
{
	extern Registry registry;

	class AnimationSystem : public System
	{
	public:
		AnimationSystem()
		{
			RequireComponent<SpriteComponent>();
			RequireComponent<AnimationComponent>();
		}

		void Update(const float deltaTime) const
		{
			for (const auto entity : GetEntities())
			{
				auto& sprite = registry.GetComponent<SpriteComponent>(entity);
				auto& animation = registry.GetComponent<AnimationComponent>(entity);

				animation.m_frameTime += deltaTime * static_cast<float>(animation.m_speed);
				animation.m_currentFrame = static_cast<int>(animation.m_frameTime) % animation.m_frames;
				sprite.m_srcRect.x = animation.m_currentFrame * sprite.m_width;
			}
		}
	};
}

