#pragma once

#include <bitset>
#include <deque>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../events/EventBus.h"
#include "../helpers/Constants.h"
#include "../helpers/Logger.h"

class Registry;

typedef std::bitset<MAX_COMPONENTS> Signature;

class Entity
{
private:
	int m_id;
	Registry* m_registry;

public:
	explicit Entity(const int id, Registry* registry) : m_id(id), m_registry(registry) {}

	template <typename T, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename T> void RemoveComponent() const;
	template <typename T> [[nodiscard]] bool HasComponent() const;
	template <typename T> T& GetComponent() const;

	[[nodiscard]] int GetId() const { return m_id; }
	[[nodiscard]] Registry* GetRegistry() const { return m_registry; }
	void Destroy() const;

	bool operator ==(const Entity& other) const { return m_id == other.m_id; }
	bool operator !=(const Entity& other) const { return m_id != other.m_id; }
	bool operator >(const Entity& other) const { return m_id > other.m_id; }
	bool operator <(const Entity& other) const { return m_id < other.m_id; }
};

class IComponent
{
protected:
	static int m_nextId;
};

template <typename T>
class Component : public IComponent
{
public:
	static int GetId()
	{
		static int id = m_nextId++;
		return id;
	}
};

class System
{
private:
	Signature m_signature;
	std::vector<Entity> m_entities;

public:
	System() = default;
	~System() = default;

	void AddEntity(Entity entity);
	void RemoveEntity(Entity entity);
	[[nodiscard]] std::vector<Entity> GetEntities() const;
	[[nodiscard]] const Signature& GetSignature() const;

	template <typename T> void RequireComponent();
};

class IPool
{
public:
	virtual ~IPool() = default;
	virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T>
class Pool final : public IPool
{
private:
	std::vector<T> m_data;
	int m_size;

	// Helper map to keep track of entity ids per index so the data is always packed
	std::unordered_map<int, int> m_entityIdToIndex;
	std::unordered_map<int, int> m_indexToEntityId;

public:
	explicit Pool(int size = 100) : m_size(0) { m_data.resize(size); }

	~Pool() override = default;

	[[nodiscard]] bool IsEmpty() const { return m_size == 0; }

	[[nodiscard]] int GetSize() const { return m_size; }

	void Resize(int n) { m_data.resize(n); }

	void Clear() { m_data.clear(); m_size = 0; }

	void Add(T object) { m_data.push_back(object); }

	void Set(const int entityId, T object)
	{
		if(m_entityIdToIndex.contains(entityId))
		{
			// If the element already exists, simply replace the component object
			const int index = m_entityIdToIndex.at(entityId);
			m_data[index] = object;
		}
		else
		{
			// When adding a new object, we keep track of the entity ids and their vector index
			const int index = m_size;
			m_entityIdToIndex.emplace(entityId, index);
			m_indexToEntityId.emplace(index, entityId);
			if(index >= m_data.size())
				m_data.resize(m_size * 2);

			m_data[index] = object;
			m_size++;
		}
	}

	void Remove(const int entityId)
	{
		// Copy the last element to the deleted position to keep the array packed
		const int indexOfRemoved = m_entityIdToIndex.at(entityId);
		const int indexOfLast = m_size - 1;
		m_data[indexOfRemoved] = m_data[indexOfLast];

		// Update the index-entity maps to point to the correct elements
		const int entityIdOfLastElement = m_indexToEntityId.at(indexOfLast);
		m_entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
		m_indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

		m_entityIdToIndex.erase(entityId);
		m_indexToEntityId.erase(indexOfLast);

		m_size--;
	}

	T& Get(const int entityId)
	{
		const int index = m_entityIdToIndex.at(entityId);
		return static_cast<T&>(m_data[index]);
	}

	void RemoveEntityFromPool(const int entityId) override
	{
		if(m_entityIdToIndex.contains(entityId))
			Remove(entityId);
	}
};


class Registry
{
private:
	int m_numEntities = 0;
	std::vector<std::shared_ptr<IPool>> m_componentPools;
	std::vector<Signature> m_entitySignatures;
	std::unordered_map<std::type_index, std::shared_ptr<System>> m_systems;

	std::set<Entity> m_entitiesToAdd;
	std::set<Entity> m_entitiesToDestroy;

	std::deque<int> m_freeIds;
	std::shared_ptr<EventBus> m_eventBus;

public:
	Registry() = default;

	explicit Registry(std::shared_ptr<EventBus> eventBus) : m_eventBus(std::move(eventBus))
	{}

	void Update();

	Entity CreateEntity();
	void DestroyEntity(Entity entity);

	template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename T> void RemoveComponent(Entity entity);
	template <typename T> [[nodiscard]] bool HasComponent(Entity entity) const;
	template <typename T> T& GetComponent(Entity entity) const;

	template <typename T, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename T> void RemoveSystem();
	template <typename T> [[nodiscard]] bool HasSystem() const;
	template <typename T> T& GetSystem() const;

	void AddEntityToSystems(Entity entity) const;
	void RemoveEntityFromSystems(const Entity& entity) const;
};

template <typename T>
void System::RequireComponent()
{
	const auto componentId = Component<T>::GetId();
	m_signature.set(componentId);
}

template <typename T, typename ...TArgs>
void Registry::AddComponent(const Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	if(componentId >= m_componentPools.size())
		m_componentPools.resize(componentId + 1, nullptr);

	if(!m_componentPools[componentId])
	{
		std::shared_ptr<Pool<T>> newComponentPool = std::make_shared<Pool<T>>();
		m_componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(m_componentPools[componentId]);

	T newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);

	m_entitySignatures[entityId].set(componentId);

	//Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id: " + std::to_string(entityId));
}

template<typename T>
void Registry::RemoveComponent(const Entity entity)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(m_componentPools[componentId]);
	componentPool->Remove(entityId);

	m_entitySignatures[entityId].set(componentId, false);

	//Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id: " + std::to_string(entityId));
}

template <typename T>
bool Registry::HasComponent(const Entity entity) const
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	return m_entitySignatures[entityId].test(componentId);
}

template <typename T>
T& Registry::GetComponent(const Entity entity) const
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	auto componentPool = std::static_pointer_cast<Pool<T>>(m_componentPools[componentId]);

	return componentPool->Get(entityId);
}

template <typename T, typename ... TArgs>
void Registry::AddSystem(TArgs&&... args)
{
	std::shared_ptr<T> newSystem = std::make_shared<T>(std::forward<TArgs>(args)...);
	m_systems.insert(std::make_pair(std::type_index(typeid(T)), newSystem));
}

template <typename T>
void Registry::RemoveSystem()
{
	const auto system = m_systems.find(std::type_index(typeid(T)));
	m_systems.erase(system);
}

template <typename T>
bool Registry::HasSystem() const
{
	return m_systems.contains(std::type_index(typeid(T)));
}

template <typename T>
T& Registry::GetSystem() const
{
	const auto system = m_systems.find(std::type_index(typeid(T)));
	return *(std::static_pointer_cast<T>(system->second));
}

template <typename T, typename ... TArgs>
void Entity::AddComponent(TArgs&&... args)
{
	m_registry->AddComponent<T>(*this, std::forward<TArgs>(args)...);
}

template <typename T>
void Entity::RemoveComponent() const
{
	m_registry->RemoveComponent<T>(*this);
}

template <typename T>
bool Entity::HasComponent() const
{
	return m_registry->HasComponent<T>(*this);
}

template <typename T>
T& Entity::GetComponent() const
{
	return m_registry->GetComponent<T>(*this);
}
