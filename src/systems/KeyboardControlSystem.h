#pragma once

#include "../components/KeyboardControlComponent.h"
#include "../core/System.h"
#include "../events/KeyPressedEvent.h"

namespace re
{
	extern Registry registry;

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
			for (const auto entity : GetEntities())
			{
				const auto& keyboard = registry.GetComponent<KeyboardControlComponent>(entity);
				auto& sprite = registry.GetComponent<SpriteComponent>(entity);
				auto& rigidBody = registry.GetComponent<RigidBodyComponent>(entity);

				switch (event.m_symbol)
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
				default:;
				}
			}
		}
	};
}
