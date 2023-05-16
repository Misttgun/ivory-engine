#include "Window.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "Ivory/Helpers/Log.h"
#include "Ivory/Input/Input.h"

namespace Ivory
{
	int32 Window::m_width = 0;
	int32 Window::m_height = 0;

	void Window::Init(const std::string& title, const glm::ivec2& size)
	{
		m_title = title;
		m_isFullscreen = false;

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			IV_CORE_ERROR("Error initializing SDL.");
			return;
		}

		if (TTF_Init() != 0)
		{
			IV_CORE_ERROR("Error initializing SDL TTF.");
			return;
		}

		m_windowedWidth = m_width = size.x;
		m_windowedHeight = m_height = size.y;

		m_window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowedWidth, m_windowedHeight,  0);
		if (!m_window)
		{
			IV_CORE_ERROR("Error creating SDL window.");
			return;
		}

		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);
		if (!m_renderer)
		{
			IV_CORE_ERROR("Error creating SDL renderer.");
			return;
		}

		// Initialize ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backend
		ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
		ImGui_ImplSDLRenderer_Init(m_renderer);

		// Init inputs
		m_input = Input::Instance();
	}

	void Window::Cleanup() const
	{
		ImGui_ImplSDLRenderer_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_window);
		SDL_Quit();
	}

	void Window::BeginDraw() const
	{
		SDL_SetRenderDrawColor(m_renderer, 21, 21, 21, 255);
		SDL_RenderClear(m_renderer);
	}

	void Window::EndDraw() const
	{
		SDL_RenderPresent(m_renderer);
	}

	void Window::ToggleFullscreen()
	{
		m_isFullscreen = !m_isFullscreen;

		if (m_isFullscreen)
		{
			SDL_DisplayMode displayMode;
			SDL_GetCurrentDisplayMode(0, &displayMode);
			m_width = displayMode.w;
			m_height = displayMode.h;
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
		}
		else
		{
			m_width = m_windowedWidth;
			m_height = m_windowedHeight;
			SDL_SetWindowFullscreen(m_window, 0);
			SDL_SetWindowSize(m_window, m_width, m_height);
			SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
	}

	void Window::ProcessEvents()
	{
		m_input->ClearState();

		SDL_Event sdlEvent{};
		while (SDL_PollEvent(&sdlEvent))
		{
			ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
			ImGuiIO& io = ImGui::GetIO();
			int32 mouseX, mouseY;
			const auto buttons = SDL_GetMouseState(&mouseX, &mouseY);
			io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
			io.MouseDown[0] = buttons && SDL_BUTTON(SDL_BUTTON_LEFT);
			io.MouseDown[1] = buttons && SDL_BUTTON(SDL_BUTTON_RIGHT);
			io.MouseDown[2] = buttons && SDL_BUTTON(SDL_BUTTON_MIDDLE);

			if(io.WantCaptureKeyboard || io.WantCaptureMouse)
				return;

			m_input->ProcessEvents(sdlEvent);

			switch (sdlEvent.type)
			{
			case SDL_QUIT:
				m_shouldClose = true;
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
					m_shouldClose = true;
				if (sdlEvent.key.keysym.sym == SDLK_F1)
					ToggleFullscreen();
				//m_eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
				break;
			default:
				break;
			}
		}
	}
}
