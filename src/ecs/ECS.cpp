#include "ECS.h"
#include "../helpers/Logger.h"

int IComponent::m_nextId = 0;

void Entity::Destroy() const
{
	m_registry->DestroyEntity(*this);
}

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

	for(const auto entity : m_entitiesToDestroy)
	{
		const int entityId = entity.GetId();

		RemoveEntityFromSystems(entity);

		for (const auto& pool : m_componentPools)
		{
			if(pool == nullptr)
				continue;

			pool->RemoveEntityFromPool(entityId);
		}

		m_entitySignatures[entityId].reset();
		m_freeIds.push_back(entityId);

		//Logger::Log("Entity with id = " + std::to_string(entityId) + " was destroyed.");
	}

	m_entitiesToDestroy.clear();
}

Entity Registry::CreateEntity()
{
	int entityId;

	if(m_freeIds.empty())
	{
		entityId = m_numEntities++;
		if(entityId >= static_cast<int>(m_entitySignatures.size()))
			m_entitySignatures.resize(entityId + 1);
	}
	else
	{
		entityId = m_freeIds.front();
		m_freeIds.pop_front();
	}

	const Entity entity(entityId, this);
	m_entitiesToAdd.insert(entity);

	//Logger::Log("Entity created with id = " + std::to_string(entityId));

	return entity;
}

void Registry::DestroyEntity(const Entity entity)
{
	m_entitiesToDestroy.insert(entity);
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

void Registry::RemoveEntityFromSystems(const Entity& entity) const
{
	for(auto [type, system] : m_systems)
		system->RemoveEntity(entity);
}
