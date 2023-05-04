#pragma once

#include <memory>
#include "../engine/core/ResourceManager.h"
#include "../engine/core/ecs/Registry.h"
#include "../engine/events/EventBus.h"
#include "../engine/helpers/Timer.h"
#include "../engine/window/Window.h"

namespace re
{
	class Registry;

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
		void Render() const;

	private:
		SDL_Rect m_camera;

		std::unique_ptr<Registry> m_registry;
		std::unique_ptr<ResourceManager> m_resourceManager;
		std::shared_ptr<EventBus> m_eventBus;
		std::shared_ptr<Window> m_window;

		bool m_isRunning;
		bool m_isDebug;

		// Debug variables
		std::string m_fps;
		uint64 m_totalFrames = 0;
		mutable Entity m_fpsCounter{};
		utils::Timer m_fpsTimer;
		utils::Timer m_updateTimer;

		uint64 m_previousFrameTime;
		float m_accumulator;
	};
}
