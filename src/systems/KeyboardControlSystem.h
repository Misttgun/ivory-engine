#pragma once

#include "../ecs/ECS.h"
#include "../events/KeyPressedEvent.h"

class EventBus;

class KeyboardControlSystem : public System
{
public:
	KeyboardControlSystem() = default;

	void SubscribeToEvent(const std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
	}

	void Update()
	{

	}

private:
	void OnKeyPressed(KeyPressedEvent& event)
	{
		std::string keyCode = std::to_string(event.m_symbol);
		std::string keySymbol(1, event.m_symbol);
		Logger::Log("Key pressed: [" + keyCode + "] " + keySymbol);
	}
};
