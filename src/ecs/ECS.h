#pragma once

#include <bitset>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

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
		static auto id = m_nextId++;
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
	[[nodiscard]] const std::vector<Entity>& GetEntities() const;
	[[nodiscard]] const Signature& GetSignature() const;

	template <typename T> void RequireComponent();
};

class IPool
{
public:
	virtual ~IPool() = default;
};

template <typename T>
class Pool final : public IPool
{
private:
	std::vector<T> m_data;

public:
	explicit Pool(int size = 100) { m_data.resize(size); }

	~Pool() override = default;

	[[nodiscard]] bool IsEmpty() const { return m_data.empty(); }

	[[nodiscard]] int GetSize() const { return static_cast<int>(m_data.size()); }

	void Resize(int n) { m_data.resize(n); }

	void Clear() { m_data.clear(); }

	void Add(T object) { m_data.push_back(object); }

	void Set(int index, T object) { m_data[index] = object; }

	T& Get(int index) { return static_cast<T&>(m_data[index]); }

	T& operator [](unsigned int index) { return m_data[index]; }
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

public:
	Registry() = default;

	void Update();

	Entity CreateEntity();

	template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename T> void RemoveComponent(Entity entity);
	template <typename T> [[nodiscard]] bool HasComponent(Entity entity) const;
	template <typename T> T& GetComponent(Entity entity) const;

	template <typename T, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename T> void RemoveSystem();
	template <typename T> [[nodiscard]] bool HasSystem() const;
	template <typename T> T& GetSystem() const;

	void AddEntityToSystems(Entity entity) const;
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

	if (componentId >= m_componentPools.size())
		m_componentPools.resize(componentId + 1, nullptr);

	if (!m_componentPools[componentId])
	{
		std::shared_ptr<Pool<T>> newComponentPool = std::make_shared<Pool<T>>();
		m_componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(m_componentPools[componentId]);

	if (entityId >= componentPool->GetSize())
	{
		componentPool->Resize(m_numEntities);
	}

	T newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);

	m_entitySignatures[entityId].set(componentId);

	Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id: " + std::to_string(entityId));
}

template<typename T>
void Registry::RemoveComponent(const Entity entity)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	m_entitySignatures[entityId].set(componentId, false);

	Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id: " + std::to_string(entityId));
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
