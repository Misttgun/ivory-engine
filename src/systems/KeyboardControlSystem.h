#pragma once

#include "../components/KeyboardControlComponent.h"
#include "../ecs/ECS.h"
#include "../events/KeyPressedEvent.h"

class EventBus;

class KeyboardControlSystem : public System
{
public:
	KeyboardControlSystem()
	{
		RequireComponent<KeyboardControlComponent>();
		RequireComponent<RigidBodyComponent>();
		RequireComponent<SpriteComponent>();
	}

	void SubscribeToEvent(const std::shared_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
	}

	void Update()
	{

	}

private:
	void OnKeyPressed(const KeyPressedEvent& event)
	{
		for(auto entity : GetEntities())
		{
			const auto& keyboard = entity.GetComponent<KeyboardControlComponent>();
			auto& sprite = entity.GetComponent<SpriteComponent>();
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			switch(event.m_symbol)
			{
				case SDLK_UP:
				case SDLK_z:
					rigidBody.m_velocity = keyboard.m_up;
					sprite.m_srcRect.y = sprite.m_height * 0;
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					rigidBody.m_velocity = keyboard.m_right;
					sprite.m_srcRect.y = sprite.m_height * 1;
					break;
				case SDLK_DOWN:
				case SDLK_s:
					rigidBody.m_velocity = keyboard.m_down;
					sprite.m_srcRect.y = sprite.m_height * 2;
					break;
				case SDLK_LEFT:
				case SDLK_q:
					rigidBody.m_velocity = keyboard.m_left;
					sprite.m_srcRect.y = sprite.m_height * 3;
					break;
				default: ;
			}
		}
	}
};
