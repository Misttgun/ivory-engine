#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "../ecs/ECS.h"
#include "../events/EventBus.h"
#include "../helpers/AssetStore.h"

class Game
{
private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Rect m_camera;

	std::unique_ptr<Registry> m_registry;
	std::unique_ptr<AssetStore> m_assetStore;
	std::shared_ptr<EventBus> m_eventBus;

	bool m_isRunning;
	bool m_isDebug;

	int m_previousFrameTime;

	void LoadLevel(int level) const;
	void Setup() const;
	void ProcessInputs();
	void Update();
	void Render() const;

public:
	Game();
	~Game() = default;

	void Init();
	void Run();
	void Destroy() const;

	static int m_windowWidth;
	static int m_windowHeight;
	static int m_mapWidth;
	static int m_mapHeight;
};

