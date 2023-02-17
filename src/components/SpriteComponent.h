#pragma once

struct SpriteComponent
{
	int m_width;
    int m_height;

    SpriteComponent() = default;

    explicit SpriteComponent(const int width, const int height) : m_width(width), m_height(height)
    {}
};
