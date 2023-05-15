#pragma once

#include "Ivory/ECS/Registry.h"
#include "Ivory/ECS/System.h"
#include "Ivory/ECS/components/AnimationComponent.h"
#include "Ivory/ECS/components/SpriteComponent.h"

namespace Ivory
{
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
			for (const auto entity : m_entities)
			{
				auto& sprite = m_registry->GetComponent<SpriteComponent>(entity);
				auto& animation = m_registry->GetComponent<AnimationComponent>(entity);

				animation.m_frameTime += deltaTime * static_cast<float>(animation.m_speed);
				animation.m_currentFrame = static_cast<int>(animation.m_frameTime) % animation.m_frames;
				sprite.m_srcRect.x = animation.m_currentFrame * sprite.m_width;
			}
		}
	};
}
