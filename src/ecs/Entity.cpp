#include "Entity.h"

Entity::Entity(int id) : m_id(id)
{}

int Entity::GetId() const
{
    return m_id;
}
