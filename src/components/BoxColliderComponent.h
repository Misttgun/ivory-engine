#pragma once

#include <glm/vec2.hpp>

namespace re
{
	struct BoxColliderComponent
	{
		glm::vec2 m_size;
		glm::vec2 m_offset;
		SDL_Color m_debugColor{};
		std::vector<Entity> m_collidingEntities;

		BoxColliderComponent() = default;

		explicit BoxColliderComponent(const glm::vec2& size, const glm::vec2& offset = glm::vec2(0)) : m_size(size), m_offset(offset)
		{
			m_debugColor = { 255, 255, 0, 255 };
		}

		void AddCollidingEntity(const Entity entity)
		{
			if (IsCollidingWithEntity(entity))
				return;

			m_collidingEntities.push_back(entity);
		}

		[[nodiscard]] bool IsCollidingWithEntity(const Entity entity) const
		{
			return std::ranges::find(m_collidingEntities, entity) != m_collidingEntities.end();
		}

		[[nodiscard]] bool IsCollidingWithAnyEntity() const
		{
			return !m_collidingEntities.empty();
		}

		void RemoveCollidingEntity(const Entity entity)
		{
			int32 index = -1;
			for (std::size_t i = 0; i < m_collidingEntities.size(); ++i)
			{
				if (m_collidingEntities.at(i) == entity)
				{
					index = static_cast<int>(i);
					break;
				}
			}

			if (index == -1)
				return;

			m_collidingEntities.erase(m_collidingEntities.begin() + index);
		}

		void ClearCollisionData()
		{
			m_collidingEntities.clear();
		}
	};
}
