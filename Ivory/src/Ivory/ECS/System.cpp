#include "System.h"

namespace Ivory
{
	void System::AddEntity(const Entity entity)
	{
		m_entities.insert(entity);
	}

	void System::RemoveEntity(const Entity entity)
	{
		m_entities.erase(entity);
	}

	const Signature& System::GetSignature() const
	{
		return m_signature;
	}

	void System::SetRegistry(Registry* registry)
	{
		m_registry = registry;
	}
}
