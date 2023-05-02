#pragma once

#include "Event.h"

namespace re
{
	class DestroyEvent : public Event
	{
	public:
		Entity m_destroyedEntity;

		explicit DestroyEvent(const Entity destroyedEntity) : m_destroyedEntity(destroyedEntity)
		{}
	};
}
