#pragma once

#include <memory>
#include <SDL2/SDL.h>

#include "../core/Registry.h"
#include "../core/ResourceManager.h"
#include "../events/EventBus.h"

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
		void LoadLevel(int32 level) const;
		void Setup() const;
		void ProcessInputs();
		void Update();
		void Render() const;

	public:
		static int32 m_windowWidth;
		static int32 m_windowHeight;
		static int32 m_mapWidth;
		static int32 m_mapHeight;

	private:
		SDL_Window* m_window;
		SDL_Renderer* m_renderer;
		SDL_Rect m_camera;

		std::unique_ptr<Registry> m_registry;
		std::unique_ptr<ResourceManager> m_resourceManager;
		std::shared_ptr<EventBus> m_eventBus;

		bool m_isRunning;
		bool m_isDebug;

		uint32 m_previousFrameTime;
		float m_accumulator;
	};
}
