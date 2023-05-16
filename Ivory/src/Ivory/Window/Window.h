#pragma once

#include <SDL_render.h>
#include <string>

#include <glm/vec2.hpp>

#include "Ivory/Types.h"

namespace Ivory
{
	class Input;

	class Window
	{
	public:
		void Init(const std::string& title, const glm::ivec2& size);
		void Cleanup() const;

		void BeginDraw() const;
		void EndDraw() const;

		[[nodiscard]] bool IsFullscreen() const { return m_isFullscreen; }
		[[nodiscard]] bool ShouldClose() const { return m_shouldClose; }
		[[nodiscard]] SDL_Renderer* Renderer() const { return m_renderer; }

		void ToggleFullscreen();
		void ProcessEvents();

	public:
		static int32 m_width;
		static int32 m_height;

	private:
		SDL_Window* m_window = nullptr;
		SDL_Renderer* m_renderer = nullptr;

		Input* m_input = nullptr;

		std::string m_title;

		bool m_isFullscreen = false;
		bool m_shouldClose = false;

		int32 m_windowedWidth = 0;
		int32 m_windowedHeight = 0;
	};
}
