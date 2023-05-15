#pragma once

#include <array>
#include <bitset>
#include <memory>
#include <queue>
#include <set>
#include <typeindex>
#include <unordered_map>

#include "ComponentPool.h"
#include "System.h"

#include "Ivory/Types.h"
#include "Ivory/Events/EventBus.h"

namespace Ivory
{
	class Registry
	{
	public:
		Registry() = default;

		void Init(const std::shared_ptr<EventBus>& eventBus);
		void Update();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);

		template <typename T>
		void AddComponent(Entity entity, T component);

		template <typename T>
		void RemoveComponent(Entity entity);

		template <typename T>
		[[nodiscard]] bool HasComponent(Entity entity) const;

		template <typename T>
		T& GetComponent(Entity entity) const;

		template <typename T>
		void RegisterSystem();

		template <typename T>
		void RemoveSystem();

		template <typename T>
		[[nodiscard]] bool HasSystem() const;

		template <typename T>
		T& GetSystem() const;

		void AddEntityToSystems(Entity entity) const;
		void RemoveEntityFromSystems(Entity entity) const;
		void EntitySignatureChanged(Entity entity) const;

	private:
		std::unordered_map<std::type_index, std::shared_ptr<IComponentPool>> m_componentPools;
		std::unordered_map<std::type_index, std::shared_ptr<System>> m_systems;

		std::set<Entity> m_entitiesToAdd;
		std::set<Entity> m_entitiesToDestroy;

		std::queue<Entity> m_freeIds; // Queue of unused entity IDs
		std::array<Signature, MAX_ENTITIES> m_entitySignatures;
		// Array of signatures where the index corresponds to the entity ID
		uint32 m_entityCount{};

		std::shared_ptr<EventBus> m_eventBus;
	};

	template <typename T>
	void Registry::AddComponent(const Entity entity, T component)
	{
		const auto typeIndex = std::type_index(typeid(T));

		if (m_componentPools.find(typeIndex) == m_componentPools.end())
			m_componentPools.insert(std::make_pair(typeIndex, std::make_shared<ComponentPool<T>>()));

		const auto componentType = Component<T>::GetType();

		const std::shared_ptr<ComponentPool<T>> componentPool = std::static_pointer_cast<ComponentPool<T>>(m_componentPools.at(typeIndex));

		componentPool->Add(entity, component);

		m_entitySignatures[entity].set(componentType);

		EntitySignatureChanged(entity);
	}

	template <typename T>
	void Registry::RemoveComponent(const Entity entity)
	{
		const auto typeIndex = std::type_index(typeid(T));

		const auto componentType = Component<T>::GetType();

		const std::shared_ptr<ComponentPool<T>> componentPool = std::static_pointer_cast<ComponentPool<T>>(m_componentPools.at(typeIndex));
		componentPool->Remove(entity);

		m_entitySignatures[entity].set(componentType, false);

		EntitySignatureChanged(entity);
	}

	template <typename T>
	bool Registry::HasComponent(const Entity entity) const
	{
		const auto componentType = Component<T>::GetType();
		return m_entitySignatures[entity].test(componentType);
	}

	template <typename T>
	T& Registry::GetComponent(const Entity entity) const
	{
		const std::shared_ptr<ComponentPool<T>> componentPool = std::static_pointer_cast<ComponentPool<T>>(m_componentPools.at(std::type_index(typeid(T))));
		return componentPool->Get(entity);
	}

	template <typename T>
	void Registry::RegisterSystem()
	{
		const auto typeIndex = std::type_index(typeid(T));

		SDL_assert(m_systems.find(typeIndex) == m_systems.end() && "Registering system more than once.");

		const std::shared_ptr<System> system = std::make_shared<T>();
		system->SetRegistry(this);
		m_systems.insert(std::make_pair(typeIndex, std::move(system)));
	}

	template <typename T>
	void Registry::RemoveSystem()
	{
		const auto typeIndex = std::type_index(typeid(T));

		SDL_assert(m_systems.find(typeIndex) != m_systems.end() && "Trying to remove system before registered.");

		const auto system = m_systems.find(typeIndex);
		m_systems.erase(system);
	}

	template <typename T>
	bool Registry::HasSystem() const
	{
		return m_systems.find(std::type_index(typeid(T))) != m_systems.end();
	}

	template <typename T>
	T& Registry::GetSystem() const
	{
		const auto typeIndex = std::type_index(typeid(T));

		SDL_assert(m_systems.find(typeIndex) != m_systems.end() && "System used before registered.");

		const auto system = m_systems.find(typeIndex);
		return *(std::static_pointer_cast<T>(system->second));
	}
}
