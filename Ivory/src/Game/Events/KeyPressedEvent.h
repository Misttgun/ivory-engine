#pragma once

namespace Ivory
{
	class KeyPressedEvent : public Event
	{
	public:
		SDL_Keycode m_symbol;

		explicit KeyPressedEvent(const SDL_Keycode symbol) : m_symbol(symbol) { }
	};
}
