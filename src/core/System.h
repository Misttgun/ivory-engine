#pragma once

#include <set>
#include "Component.h"
#include "Types.h"

namespace re
{
	class System
	{
	public:
		System() = default;

		void AddEntity(Entity entity);
		void RemoveEntity(Entity entity);
		[[nodiscard]] std::set<Entity> GetEntities() const;
		[[nodiscard]] const Signature& GetSignature() const;

		template <typename T> void RequireComponent();

	private:
		Signature m_signature;
		std::set<Entity> m_entities;
	};

	template <typename T>
	void System::RequireComponent()
	{
		const auto componentId = Component<T>::GetType();
		m_signature.set(componentId);
	}
}
