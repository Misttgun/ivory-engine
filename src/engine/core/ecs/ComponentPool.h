#pragma once

#include <array>
#include <unordered_map>
#include <SDL2/SDL_assert.h>
#include "../Types.h"

namespace re
{
	class IComponentPool
	{
	public:
		virtual ~IComponentPool() = default;
		IComponentPool(const IComponentPool& pool) = delete;
		IComponentPool(IComponentPool&& pool) = delete;
		IComponentPool& operator=(const IComponentPool& pool) = delete;
		IComponentPool& operator=(IComponentPool&& pool) = delete;

		virtual void RemoveEntityFromPool(Entity entity) = 0;

	protected:
		IComponentPool() = default;
	};

	template <typename T>
	class ComponentPool final : public IComponentPool
	{
	public:
		ComponentPool() = default;

		void Add(const Entity entity, T component)
		{
			SDL_assert(m_entityIdToIndex.contains(entity) == false && "Component added to same entity more than once.");

			// Put new entry at end
			size_t index = m_size;
			m_entityIdToIndex.emplace(entity, index);
			m_indexToEntityId.emplace(index, entity);
			m_componentArray[index] = component;
			++m_size;
		}

		void Remove(const Entity entity)
		{
			SDL_assert(m_entityIdToIndex.contains(entity) && "Removing non-existent component.");

			// Copy element at end into deleted element's place to maintain density
			const auto indexOfRemoved = m_entityIdToIndex.at(entity);
			const auto indexOfLast = m_size - 1;
			m_componentArray[indexOfRemoved] = m_componentArray[indexOfLast];

			// Update the index-entity maps to point to the correct elements
			const auto entityIdOfLastElement = m_indexToEntityId.at(indexOfLast);
			m_entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
			m_indexToEntityId[indexOfRemoved] = entityIdOfLastElement;
			m_entityIdToIndex.erase(entity);
			m_indexToEntityId.erase(indexOfLast);

			m_size--;
		}

		T& Get(const Entity entity)
		{
			SDL_assert(m_entityIdToIndex.contains(entity) && "Retrieving non-existent component.");

			const auto index = m_entityIdToIndex.at(entity);
			return m_componentArray[index];
		}

		void RemoveEntityFromPool(const Entity entity) override
		{
			if (m_entityIdToIndex.contains(entity) == false)
				return;

			Remove(entity);
		}

	private:
		size_t m_size{};
		std::array<T, MAX_ENTITIES> m_componentArray{};

		// Helper map to keep track of entity ids per index so the data is always packed
		std::unordered_map<uint32, size_t> m_entityIdToIndex{};
		std::unordered_map<size_t, uint32> m_indexToEntityId{};
	};
}
