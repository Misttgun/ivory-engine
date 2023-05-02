#pragma once

#include <set>
#include "Component.h"
#include "Types.h"

namespace re
{
	class Registry;

	class System
	{
	public:
		System() = default;

		void AddEntity(Entity entity);
		void RemoveEntity(Entity entity);
		[[nodiscard]] const Signature& GetSignature() const;

		void SetRegistry(Registry* registry);

		template <typename T>
		void RequireComponent();

	private:
		Signature m_signature;

	protected:
		Registry* m_registry{};
		std::set<Entity> m_entities;
	};

	template <typename T>
	void System::RequireComponent()
	{
		const auto componentId = Component<T>::GetType();
		m_signature.set(componentId);
	}
}
