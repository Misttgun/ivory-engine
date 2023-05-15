#pragma once

#include <SDL_render.h>
#include <string>

#include <glm/vec2.hpp>

#include "Ivory/Types.h"

namespace Ivory
{
	class Window
	{
	public:
		void Init(const std::string& title, const glm::ivec2& size);
		void Cleanup() const;

		void BeginDraw() const;
		void EndDraw() const;

		[[nodiscard]] bool IsFullscreen() const { return m_isFullscreen; }
		[[nodiscard]] SDL_Renderer* Renderer() const { return m_renderer; }

		void ToggleFullscreen();

	public:
		static int32 m_width;
		static int32 m_height;

	private:
		SDL_Window* m_window = nullptr;
		SDL_Renderer* m_renderer = nullptr;

		std::string m_title;

		bool m_isFullscreen = false;
		int32 m_windowedWidth = 0;
		int32 m_windowedHeight = 0;
	};
}
