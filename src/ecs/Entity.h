#pragma once

class Entity
{
private:
	int m_id;

public:
	Entity(int id);
	int GetId() const;

	bool operator ==(const Entity& other) const { return m_id == other.m_id; }
	bool operator !=(const Entity& other) const { return m_id != other.m_id; }
};

