#pragma once

#include <glm/vec2.hpp>

struct BoxColliderComponent
{
	glm::vec2 m_size;
	glm::vec2 m_offset;
	SDL_Color m_debugColor{};
	std::vector<int> m_collidingIds;

	BoxColliderComponent() = default;

	explicit BoxColliderComponent(const glm::vec2& size, const glm::vec2& offset = glm::vec2(0)) : m_size(size), m_offset(offset)
	{
		m_debugColor = {255, 255, 0, 255};
	}

	void AddCollidingEntity(const int entityId)
	{
		if(IsCollidingWithEntity(entityId))
			return;

		m_collidingIds.push_back(entityId);
	}

	[[nodiscard]] bool IsCollidingWithEntity(const int entityId) const
	{
		return std::ranges::find(m_collidingIds, entityId) != m_collidingIds.end();
	}

	[[nodiscard]] bool IsCollidingWithAnyEntity() const
	{
		return !m_collidingIds.empty();
	}

	void RemoveCollidingEntity(const int entityId)
	{
		int index = -1;
		for (std::size_t i = 0; i < m_collidingIds.size(); ++i)
		{
			if(m_collidingIds.at(i) == entityId)
			{
				index = static_cast<int>(i);
				break;
			}
		}

		if(index == -1)
			return;

		m_collidingIds.erase(m_collidingIds.begin() + index);
	}

	void ClearCollisionData()
	{
		m_collidingIds.clear();
	}
};
