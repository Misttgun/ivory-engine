#include "Registry.h"
#include <SDL2/SDL_assert.h>
#include "../events/DestroyEvent.h"

namespace re
{
	void Registry::Init(const std::shared_ptr<EventBus>& eventBus)
	{
		m_eventBus = eventBus;

		// Initialize the queue with all possible entity IDs
		for (uint32 id = 0; id < MAX_ENTITIES; ++id)
			m_freeIds.push(id);
	}

	void Registry::Update()
	{
		for (const auto entity : m_entitiesToAdd)
			AddEntityToSystems(entity);

		m_entitiesToAdd.clear();

		for (const auto& entity : m_entitiesToDestroy)
		{
			RemoveEntityFromSystems(entity);

			for (const auto& [index, pool] : m_componentPools)
				pool->RemoveEntityFromPool(entity);

			m_entitySignatures[entity].reset();
			m_freeIds.push(entity);
		}

		m_entitiesToDestroy.clear();
	}

	Entity Registry::CreateEntity()
	{
		SDL_assert(m_entityCount < MAX_ENTITIES && "Too many entities in existence.");

		const auto entity = m_freeIds.front();
		m_freeIds.pop();
		++m_entityCount;

		m_entitiesToAdd.insert(entity);

		return entity;
	}

	void Registry::DestroyEntity(const Entity entity)
	{
		SDL_assert(entity < MAX_ENTITIES && "Entity out of range.");

		m_entitiesToDestroy.insert(entity);
		m_eventBus->EmitEvent<DestroyEvent>(entity);

		--m_entityCount;
	}

	void Registry::AddEntityToSystems(const Entity entity) const
	{
		const auto& entitySignature = m_entitySignatures[entity];

		for (const auto& [index, system] : m_systems)
		{
			const auto& systemSignature = system->GetSignature();
			const bool isInterested = (entitySignature & systemSignature) == systemSignature;

			if (isInterested)
				system->AddEntity(entity);
		}
	}

	void Registry::RemoveEntityFromSystems(const Entity entity) const
	{
		for (auto [type, system] : m_systems)
			system->RemoveEntity(entity);
	}

	void Registry::EntitySignatureChanged(const Entity entity) const
	{
		const auto& entitySignature = m_entitySignatures[entity];

		for (const auto& [index, system] : m_systems)
		{
			const auto& systemSignature = system->GetSignature();
			const bool isInterested = (entitySignature & systemSignature) == systemSignature;

			if (isInterested)
				system->AddEntity(entity);
			else
				system->RemoveEntity(entity);
		}
	}
}
