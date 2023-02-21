#pragma once

#include <SDL2/SDL_keycode.h>
#include "Event.h"

class KeyPressedEvent : public Event
{
public:
	SDL_Keycode m_symbol;

	explicit KeyPressedEvent(SDL_Keycode symbol) : m_symbol(symbol)
	{}
};
