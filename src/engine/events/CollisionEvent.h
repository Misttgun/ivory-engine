#pragma once

#include "Event.h"

namespace re
{
	class CollisionEvent : public Event
	{
	public:
		Entity m_entityA;
		Entity m_entityB;

		explicit CollisionEvent(const Entity a, const Entity b) : m_entityA(a), m_entityB(b) { }
	};
}
