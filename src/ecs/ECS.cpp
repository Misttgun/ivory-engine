#include "ECS.h"
#include "../helpers/Logger.h"

int IComponent::m_nextId = 0;

void System::AddEntity(const Entity entity)
{
	m_entities.push_back(entity);
}

void System::RemoveEntity(const Entity entity)
{
	for(std::size_t i = 0; i < m_entities.size(); i++)
	{
		if(m_entities.at(i) != entity)
			continue;
		m_entities.erase(m_entities.begin() + static_cast<ptrdiff_t>(i));
		break;
	}
}

std::vector<Entity> System::GetEntities() const
{
	return m_entities;
}

const Signature& System::GetSignature() const
{
	return m_signature;
}

void Registry::Update()
{
	for(const auto entity : m_entitiesToAdd)
		AddEntityToSystems(entity);

	m_entitiesToAdd.clear();
}

Entity Registry::CreateEntity()
{
	const auto entityId = m_numEntities++;

	const Entity entity(entityId, this);

	m_entitiesToAdd.insert(entity);

	if(entityId >= static_cast<int>(m_entitySignatures.size()))
		m_entitySignatures.resize(entityId + 1);

	Logger::Log("Entity created with id = " + std::to_string(entityId));

	return entity;
}

void Registry::AddEntityToSystems(const Entity entity) const
{
	const auto entityId = entity.GetId();
	const auto& entitySignature = m_entitySignatures[entityId];

	for(const auto& [index, system] : m_systems)
	{
		const auto& systemSignature = system->GetSignature();
		const bool isInterested = (entitySignature & systemSignature) == systemSignature;

		if(isInterested)
			system->AddEntity(entity);
	}
}
