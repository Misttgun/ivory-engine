#pragma once

#include "../components/BoxColliderComponent.h"
#include "../ecs/ECS.h"
#include "../events/CollisionEvent.h"
#include "../events/EventBus.h"

class EventBus;

class DamageSystem : public System
{
public:
	DamageSystem()
	{
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvent(const std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
	}

	void Update()
	{

	}

private:
	void OnCollision(CollisionEvent& event)
	{
		event.m_entityA.Destroy();
		event.m_entityB.Destroy();
	}
};
