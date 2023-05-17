#pragma once

#include <memory>
#include <SDL_rect.h>

#include "Ivory/ResourceManager.h"
#include "Ivory/ECS/Registry.h"
#include "Ivory/Events/EventBus.h"
#include "Ivory/helpers/Timer.h"
#include "Ivory/window/Window.h"

namespace Ivory
{
	class Game
	{
	public:
		Game();
		~Game() = default;
		Game(const Game& game) = delete;
		Game(Game&& game) = delete;
		Game& operator=(const Game& game) = delete;
		Game& operator=(Game&& game) = delete;

		void Init();
		void Run();
		void Destroy() const;

	private:
		void LoadLevel() const;
		void Setup() const;
		void ProcessInputs();
		void Update();
		void Render();

	private:
		SDL_Rect m_camera;

		std::unique_ptr<Registry> m_registry;
		std::unique_ptr<ResourceManager> m_resourceManager;
		std::shared_ptr<EventBus> m_eventBus;
		std::shared_ptr<Window> m_window;

		bool m_isDebug;

		// Debug variables
		std::string m_fps;
		uint32 m_totalFrames = 0;
		mutable Entity m_fpsCounter{};
		Timer m_fpsTimer;
		Timer m_gameTimer;
		Timer m_updateTimer;

		uint64 m_previousFrameTime;
		float m_accumulator;
	};
}
