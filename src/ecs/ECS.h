#pragma once

#include <bitset>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../helpers/Constants.h"

typedef std::bitset<MAX_COMPONENTS> Signature;

class Entity
{
private:
	int m_id;

public:
	explicit Entity(const int id) : m_id(id) {}

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
	virtual ~IPool();
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

	[[nodiscard]] int GetSize() const { return m_data.size(); }

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
	std::vector<IPool*> m_componentPools;
	std::vector<Signature> m_entitySignatures;
	std::unordered_map<std::type_index, System*> m_systems;

	std::set<Entity> m_entitiesToAdd;
	std::set<Entity> m_entitiesToDestroy;

public:
	Registry() = default;

	void Update();

	Entity CreateEntity();

	template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename T> void RemoveComponent(Entity entity);
	template <typename T> bool HasComponent(Entity entity);

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

	if(componentId >= m_componentPools.size())
		m_componentPools.resize(componentId + 1, nullptr);

	if(!m_componentPools[componentId])
	{
		Pool<T>* newComponentPool = new Pool<T>();
		m_componentPools[componentId] = newComponentPool;
	}

	Pool<T>* componentPool = m_componentPools[componentId];

	if(entityId >= componentPool->GetSize())
	{
		componentPool->Resize(m_numEntities);
	}

	T newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);

	m_entitySignatures[entityId].set(componentId);
}

template<typename T>
void Registry::RemoveComponent(const Entity entity)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	m_entitySignatures[entityId].set(componentId, false);
}

template <typename T>
bool Registry::HasComponent(const Entity entity)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	return m_entitySignatures[entityId].test(componentId);
}

template <typename T, typename ... TArgs>
void Registry::AddSystem(TArgs&&... args)
{
	T* newSystem(new T(std::forward<TArgs>(args)...));
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
