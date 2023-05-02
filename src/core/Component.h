#pragma once

#include "Types.h"

namespace re
{
	class IComponent
	{
	protected:
		static ComponentType m_nextType;
	};

	template <typename T>
	class Component : public IComponent
	{
	public:
		static ComponentType GetType()
		{
			static ComponentType type = m_nextType++;
			return type;
		}
	};
}
