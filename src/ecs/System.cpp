#include "System.h"

void System::AddEntity(Entity entity)
{
	m_entities.push_back(entity);
}

void System::RemoveEntity(Entity entity)
{
	for(std::size_t i = 0; i < m_entities.size(); i++)
	{
		if(m_entities.at(i) != entity)
			continue;

		m_entities.erase(m_entities.begin() + i);
		break;
	}
}

const std::vector<Entity>& System::GetEntities() const
{
	return m_entities;
}

const Signature& System::GetSignature() const
{
	return m_signature;
}
