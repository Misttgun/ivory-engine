#pragma once

#include <SDL_events.h>

#include <glm/vec2.hpp>

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Ivory
{
	class Input
	{
	public:
		static Input* Instance();

		Input(Input& other) = delete;
		void operator=(const Input&) = delete;

		void ClearState();
		void ProcessEvents(const SDL_Event& event);

		static bool IsKeyDown(const KeyCode key) { return s_Instance->IsKeyDownInternal(key); }
		static bool IsKeyPressed(const KeyCode key) { return s_Instance->IsKeyPressedInternal(key); }
		static bool IsKeyUp(const KeyCode key) { return s_Instance->IsKeyUpInternal(key); }

		static bool IsMouseDown(const MouseButton button) { return s_Instance->IsMouseDownInternal(button); }
		static bool IsMousePressed(const MouseButton button) { return s_Instance->IsMousePressedInternal(button); }
		static bool IsMouseUp(const MouseButton button) { return s_Instance->IsMouseUpInternal(button); }

		static glm::ivec2 MousePosition() { return s_Instance->MousePositionInternal(); }

	private:
		Input();
		~Input() = default;

		[[nodiscard]] bool IsKeyDownInternal(KeyCode key) const;
		[[nodiscard]] bool IsKeyPressedInternal(KeyCode key) const;
		[[nodiscard]] bool IsKeyUpInternal(KeyCode key) const;

		[[nodiscard]] bool IsMouseDownInternal(MouseButton button) const;
		[[nodiscard]] bool IsMousePressedInternal(MouseButton button) const;
		[[nodiscard]] bool IsMouseUpInternal(MouseButton button) const;

		[[nodiscard]] glm::ivec2 MousePositionInternal() const;

	private:
		// Current single instance of the class.
		static Input* s_Instance;

		// Saves SDL internal keyboard state.
		const uint8* m_keyboard;

		// Saves SDL internal mouse state.
		uint32 m_mouse;

		glm::ivec2 m_mousePosition;

		bool m_keyDown[KEYBOARD_SIZE];
		bool m_keyUp[KEYBOARD_SIZE];

		bool m_mouseDown[MOUSEBUTTON_SIZE];
		bool m_mouseUp[MOUSEBUTTON_SIZE];
	};
}
