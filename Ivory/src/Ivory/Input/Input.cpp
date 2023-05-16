#include "Input.h"

#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Ivory/Helpers/Log.h"

namespace Ivory
{
	Input* Input::s_Instance = nullptr;

	Input* Input::Instance()
	{
		if(s_Instance == nullptr)
			s_Instance = new Input();

		return s_Instance;
	}

	Input::Input() : m_keyboard(nullptr), m_mouse(0), m_mousePosition(0), m_keyDown{}, m_keyUp{}, m_mouseDown{}, m_mouseUp{} {}

	bool Input::IsKeyDownInternal(const KeyCode key) const
	{
		int toto = SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(key));
		return this->m_keyDown[SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(key))];
	}

	bool Input::IsKeyPressedInternal(const KeyCode key) const
	{
		if(this->m_keyboard == nullptr)
			return false;

		return this->m_keyboard[SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(key))];
	}

	bool Input::IsKeyUpInternal(const KeyCode key) const
	{
		return this->m_keyUp[SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(key))];
	}

	bool Input::IsMouseDownInternal(const MouseButton button) const
	{
		return this->m_mouseDown[static_cast<uint8>(button)];
	}

	bool Input::IsMousePressedInternal(const MouseButton button) const
	{
		switch(button)
		{
			case MouseButton::MOUSE_LEFT:
				if(this->m_mouse & SDL_BUTTON(1))
					return true;
				break;

			case MouseButton::MOUSE_RIGHT:
				if(this->m_mouse & SDL_BUTTON(3))
					return true;
				break;

			case MouseButton::MOUSE_MIDDLE:
				if(this->m_mouse & SDL_BUTTON(2))
					return true;
				break;
		}

		return false;
	}

	bool Input::IsMouseUpInternal(const MouseButton button) const
	{
		return this->m_mouseUp[static_cast<uint8>(button)];
	}

	glm::ivec2 Input::MousePositionInternal() const
	{
		return this->m_mousePosition;
	}

	void Input::ClearState()
	{
		int32 i;

		for(i = 0; i < KEYBOARD_SIZE; i++)
		{
			this->m_keyDown[i] = false;
			this->m_keyUp[i] = false;
		}

		for(i = 0; i < MOUSEBUTTON_SIZE; i++)
		{
			this->m_mouseDown[i] = false;
			this->m_mouseUp[i] = false;
		}


	}

	void Input::ProcessEvents(const SDL_Event& event)
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
			{
				this->m_keyboard = SDL_GetKeyboardState(nullptr);

				const int32 index = event.key.keysym.scancode;
				const int32 indexSym = event.key.keysym.sym;

				if(event.key.repeat == 0)
				{
					IV_CORE_INFO(indexSym);
					this->m_keyDown[index] = true;
				}
			}
			break;

			case SDL_KEYUP:
			{
				this->m_keyboard = SDL_GetKeyboardState(nullptr);

				const int32 index = event.key.keysym.scancode;

				if(event.key.repeat == 0)
					this->m_keyUp[index] = true;
			}
			break;

			case SDL_MOUSEMOTION:
				this->m_mousePosition = glm::ivec2(event.motion.x, event.motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
				this->m_mouse = SDL_GetMouseState(&(this->m_mousePosition.x), &(this->m_mousePosition.y));

				if(event.button.button == SDL_BUTTON_LEFT)
					this->m_mouseDown[static_cast<uint8>(MouseButton::MOUSE_LEFT)] = true;
				else if(event.button.button == SDL_BUTTON_RIGHT)
					this->m_mouseDown[static_cast<uint8>(MouseButton::MOUSE_RIGHT)] = true;
				else if(event.button.button == SDL_BUTTON_RIGHT)
					this->m_mouseDown[static_cast<uint8>(MouseButton::MOUSE_MIDDLE)] = true;
				break;

			case SDL_MOUSEBUTTONUP:
				this->m_mouse = SDL_GetMouseState(&(this->m_mousePosition.x), &(this->m_mousePosition.y));

				if(event.button.button == SDL_BUTTON_LEFT)
					this->m_mouseUp[static_cast<uint8>(MouseButton::MOUSE_LEFT)] = true;
				else if(event.button.button == SDL_BUTTON_RIGHT)
					this->m_mouseUp[static_cast<uint8>(MouseButton::MOUSE_RIGHT)] = true;
				else if(event.button.button == SDL_BUTTON_RIGHT)
					this->m_mouseUp[static_cast<uint8>(MouseButton::MOUSE_MIDDLE)] = true;
				break;

			default:
				break;
		}
	}
}
