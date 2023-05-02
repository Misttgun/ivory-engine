#include "System.h"

namespace re
{
	void System::AddEntity(const Entity entity)
	{
		m_entities.insert(entity);
	}

	void System::RemoveEntity(const Entity entity)
	{
		m_entities.erase(entity);
	}

	std::set<Entity> System::GetEntities() const
	{
		return m_entities;
	}

	const Signature& System::GetSignature() const
	{
		return m_signature;
	}
}
