#pragma once

#include <bitset>
#include <vector>

#include "../helpers/Constants.h"
#include "Entity.h"
#include "Component.h"

typedef std::bitset<MAX_COMPONENTS> Signature;

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
	const std::vector<Entity>& GetEntities() const;
	const Signature& GetSignature() const;

	template <typename TComponent> void RequireComponent();
};

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	m_signature.set(componentId);
}