#pragma once

#include <SDL2/SDL_keycode.h>
#include "Event.h"

namespace re
{
	class KeyPressedEvent : public Event
	{
	public:
		SDL_Keycode m_symbol;

		explicit KeyPressedEvent(const SDL_Keycode symbol) : m_symbol(symbol) { }
	};
}
