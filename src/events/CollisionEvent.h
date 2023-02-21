#pragma once

#include "Event.h"
#include "../ecs/ECS.h"

class CollisionEvent : public Event
{
public:
	Entity m_entityA;
	Entity m_entityB;

	CollisionEvent(const Entity a, const Entity b) : m_entityA(a), m_entityB(b)
	{}

};

