#pragma once
#include "Event.h"
#include "../ecs/ECS.h"


class DestroyEvent : public Event
{
public:
	Entity m_destroyedEntity;

	DestroyEvent(const Entity destroyedEntity) : m_destroyedEntity(destroyedEntity)
	{}
};
